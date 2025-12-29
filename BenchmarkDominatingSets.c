// BenchmarkDominatingSets.c
// Gera grafos e mede tempo/counters dos algoritmos de dominating sets

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Graph.h"
#include "GraphDominatingSets.h"
#include "IndicesSet.h"
#include "instrumentation.h"

// ---------- Geradores de grafos ----------

// Grafo caminho (esparso): 0-1-2-...-(n-1)
static Graph* MakePathGraph(unsigned int n) {
  Graph* g = GraphCreate(n, 0, 0); // undirected, unweighted
  for (unsigned int i = 0; i + 1 < n; i++) {
    GraphAddEdge(g, i, i + 1);
  }
  return g;
}

// Grafo aleatório Erdos-Renyi G(n,p) (não-orientado, sem pesos)
static Graph* MakeRandomGraph(unsigned int n, double p) {
  Graph* g = GraphCreate(n, 0, 0); // undirected, unweighted
  for (unsigned int i = 0; i < n; i++) {
    for (unsigned int j = i + 1; j < n; j++) {
      double r = (double)rand() / (double)RAND_MAX;
      if (r < p) {
        GraphAddEdge(g, i, j);
      }
    }
  }
  return g;
}

// ---------- Runner de benchmarks ----------

static void RunOne(const char* label, Graph* g, int runMinWeight) {
  // Mostra n e m
  unsigned int n = GraphGetNumVertices(g);
  unsigned int m = GraphGetNumEdges(g);

  printf("\n# %s  n=%u  m=%u\n", label, n, m);

  // MIN dominating set
  InstrReset();
  IndicesSet* md = GraphComputeMinDominatingSet(g);
  // imprime tamanho só para sanity check
  printf("MinDom |D|=%u\n", IndicesSetGetNumElems(md));
  IndicesSetDestroy(&md);
  InstrPrint();

  // MIN WEIGHT dominating set (opcional)
  if (runMinWeight) {
    InstrReset();
    IndicesSet* mwd = GraphComputeMinWeightDominatingSet(g);
    printf("MinWeightDom |D|=%u\n", IndicesSetGetNumElems(mwd));
    IndicesSetDestroy(&mwd);
    InstrPrint();
  }
}

int main(void) {
  // --- Config instrumentation counters ---
  // Vais usar estes nomes + contadores dentro do GraphDominatingSets.c
  InstrName[0] = "subsets_total";
  InstrName[1] = "subsets_checked";
  InstrName[2] = "isdom_calls";
  InstrCalibrate();

  srand(12345); // fixo para reprodutibilidade

  // Ajusta limites conforme o teu PC (2^n explode rápido)
  unsigned int n_min = 5;
  unsigned int n_max = 12;

  // Testes: esparso (caminho) e aleatório com p=0.2 e p=0.5
  for (unsigned int n = n_min; n <= n_max; n++) {
    Graph* g1 = MakePathGraph(n);
    RunOne("PATH", g1, 1);
    GraphDestroy(&g1);

    Graph* g2 = MakeRandomGraph(n, 0.20);
    RunOne("RAND_p0.20", g2, 1);
    GraphDestroy(&g2);

    Graph* g3 = MakeRandomGraph(n, 0.50);
    RunOne("RAND_p0.50", g3, 1);
    GraphDestroy(&g3);
  }

  return 0;
}
