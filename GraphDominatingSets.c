//
// Algoritmos e Estruturas de Dados --- 2025/2026
//
// Joaquim Madeira - Nov 2025
//
// GraphDominatingSets - Computing Vertex Dominating Sets for UNDIRECTED graphs
//

// Student Name : Keegan Azevedo
// Student Number : 110160
// Student Name :
// Student Number :

/*** COMPLETE THE GraphIsDominatingSet FUNCTION ***/
/*** COMPLETE THE GraphMinDominatingSet FUNCTION ***/
/*** COMPLETE THE GraphMinWeightDominatingSet FUNCTION ***/

#include "GraphDominatingSets.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "Graph.h"
#include "IndicesSet.h"
#include "instrumentation.h"

//
// TO BE COMPLETED
//
// Check if the given set is a dominating set for the graph
// Return 1 if true, or 0 otherwise
//
// A dominating set is a set of graph vertices such that every other
// graph vertex not in the set is adjacent to a graph vertex in the set
//
int GraphIsDominatingSet(const Graph* g, IndicesSet* vertSet) {
  assert(g != NULL);
  assert(GraphIsDigraph(g) == 0);
  assert(IndicesSetIsEmpty(vertSet) == 0);


  // vertSet tem de estar contido no conjunto de vértices do grafo
  IndicesSet* allV = GraphGetSetVertices(g);
  if (!IndicesSetIsSubset(vertSet, allV)) {
    IndicesSetDestroy(&allV);
    return 0;
  }

  // Para cada vértice v existente no grafo:
  int v = IndicesSetGetFirstElem(allV);
  while (v != -1) {
    // Se v já está no conjunto dominante, está coberto
    if (IndicesSetContains(vertSet, (uint16_t)v)) {
      v = IndicesSetGetNextElem(allV);
      continue;
    }

    // Senão, tem de ter pelo menos 1 vizinho em vertSet
    int dominated = 0;
    IndicesSet* adj = GraphGetSetAdjacentsTo(g, (unsigned int)v);

    int u = IndicesSetGetFirstElem(adj);
    while (u != -1) {
      if (IndicesSetContains(vertSet, (uint16_t)u)) {
        dominated = 1;
        break;
      }
      u = IndicesSetGetNextElem(adj);
    }

    IndicesSetDestroy(&adj);

    if (!dominated) {
      IndicesSetDestroy(&allV);
      return 0;  // encontrou um vértice não dominado
    }

    v = IndicesSetGetNextElem(allV);
  }

  IndicesSetDestroy(&allV);
  return 1;
}


//
// TO BE COMPLETED
//
// Compute a MIN VERTEX DOMINATING SET of the graph
// using an EXHAUSTIVE SEARCH approach
// Return the/a dominating set
//
IndicesSet* GraphComputeMinDominatingSet(const Graph* g) {
  assert(g != NULL);
  assert(GraphIsDigraph(g) == 0);

  unsigned int range = GraphGetVertexRange(g);

  // Conjunto dos vértices que realmente existem no grafo
  IndicesSet* allV = GraphGetSetVertices(g);

  // Melhor solução começa como o conjunto total (sempre dominante)
  IndicesSet* best = IndicesSetCreateCopy(allV);

  // Vamos enumerar todos os subconjuntos de {0..range-1}
  IndicesSet* candidate = IndicesSetCreateEmpty(range);

  do {
    InstrCount[0] += 1;   // subsets_total

    // Ignorar vazio (GraphIsDominatingSet exige não vazio)
    if (IndicesSetIsEmpty(candidate)) continue;

    // Ignorar subconjuntos que usam vértices que não existem no grafo
    if (!IndicesSetIsSubset(candidate, allV)) continue;

    // Poda: se já não pode bater o best, nem vale a pena testar
    if (IndicesSetGetNumElems(candidate) >= IndicesSetGetNumElems(best)) continue;

    InstrCount[1] += 1;   // subsets_checked
    InstrCount[2] += 1;   // isdom_calls


    // Testar se é dominante
    if (GraphIsDominatingSet(g, candidate)) {
      IndicesSetDestroy(&best);
      best = IndicesSetCreateCopy(candidate);
    }

  } while (IndicesSetNextSubset(candidate));

  IndicesSetDestroy(&candidate);
  IndicesSetDestroy(&allV);

  return best;
}


//
// TO BE COMPLETED
//
// Compute a MIN WEIGHT VERTEX DOMINATING SET of the graph
// using an EXHAUSTIVE SEARCH approach
// Return the dominating set
//
IndicesSet* GraphComputeMinWeightDominatingSet(const Graph* g) {
  assert(g != NULL);
  assert(GraphIsDigraph(g) == 0);

  unsigned int range = GraphGetVertexRange(g);

  // Conjunto dos vértices existentes no grafo
  IndicesSet* allV = GraphGetSetVertices(g);

  // Pesos por vértice (size = range). Vértices inexistentes têm -1
  double* vw = GraphComputeVertexWeights(g);

  // Melhor começa como o conjunto total (dominante garantido)
  IndicesSet* best = IndicesSetCreateCopy(allV);

  // Calcular o peso do best
  double bestWeight = 0.0;
  int v = IndicesSetGetFirstElem(best);
  while (v != -1) {
    bestWeight += vw[v];
    v = IndicesSetGetNextElem(best);
  }

  // Enumerar subconjuntos
  IndicesSet* candidate = IndicesSetCreateEmpty(range);

  do {
    InstrCount[0] += 1;   // subsets_total

    if (IndicesSetIsEmpty(candidate)) continue;
    if (!IndicesSetIsSubset(candidate, allV)) continue;

    // Calcular peso do candidate (poda: para cedo se já passar bestWeight)
    double candWeight = 0.0;
    int u = IndicesSetGetFirstElem(candidate);
    while (u != -1) {
      candWeight += vw[u];
      if (candWeight > bestWeight) break;  // poda simples
      u = IndicesSetGetNextElem(candidate);
    }
    if (candWeight > bestWeight) continue;
    
    InstrCount[1] += 1;   // subsets_checked
    InstrCount[2] += 1;   // isdom_calls

    if (GraphIsDominatingSet(g, candidate)) {
      // Melhor se tiver peso menor, ou empate de peso e menos vértices
      if (candWeight < bestWeight ||
          (candWeight == bestWeight &&
           IndicesSetGetNumElems(candidate) < IndicesSetGetNumElems(best))) {
        IndicesSetDestroy(&best);
        best = IndicesSetCreateCopy(candidate);
        bestWeight = candWeight;
      }
    }

  } while (IndicesSetNextSubset(candidate));

  free(vw);
  IndicesSetDestroy(&candidate);
  IndicesSetDestroy(&allV);

  return best;
}

