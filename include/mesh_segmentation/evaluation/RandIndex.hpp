#ifndef RAND_INDEX_HPP
#define RAND_INDEX_HPP

#include "mesh_segmentation/evaluation/Segmentations.hpp"
#include "mesh_segmentation/evaluation/Entry.hpp"


/**************************************************************************************************
 **************************************************************************************************
 * EvalMethod 4 : Rand Index related **************************************************************
 **************************************************************************************************
 **************************************************************************************************/

/*
 * Get Combination(N,2)
 */
inline int CN2(int N) {
	return N * (N - 1) / 2;
}

inline double CN2(int N, double den) {
	return N / den * (N-1) / 2;
}

/*
 * Get Rand Index
 */
Entry_RI* EvaluateRandIndex(Segmentation* s1, Segmentation* s2){
	
	// variables
	double RI = 0;

  // Number of segments in s1 and s2
  int nSeg1 = s1->getSegments().size();
  int nSeg2 = s2->getSegments().size();

  // Number of faces in segments(should be the same for s1 and s2)
  int nFaces = s1->getMesh()->numFaces();
	
	// deal with the extreme case for optimization: every face is a segmentation
	if(nSeg1 == nFaces || nSeg2==nFaces) {
		Segmentation* s;
		// s is the normal segmentation
		s = (nSeg1==nFaces)?s2:s1;
		int nSeg = s->getSegments().size();
		for(int i = 0; i < nSeg; i ++) {
			int size = s->getSegments()[i].getFaces().size();
			RI += CN2(size, nFaces);
		}
		RI /= ((nFaces-1)/2);		
	}
	// other cases
	else { 
		// construct a table
		int **n  = new int*[nSeg1];
		for(int i = 0; i < nSeg1; i ++) {
			n[i] = new int[nSeg2];
			for(int j = 0; j < nSeg2; j ++) {
				n[i][j] = 0;
			}
		}		
    for (FaceId face(0); face < nFaces; ++face) {
			int segId1 = s1->getMesh()->getFaceCluster(face);
			int segId2 = s2->getMesh()->getFaceCluster(face);
			n[segId1][segId2] ++;
		}
	
		// compute the value 1-RI
		for(int i = 0; i < nSeg1; i ++) {
			int size = s1->getSegments()[i].getFaces().size();
			RI += CN2(size, nFaces);
		}
		for(int i = 0; i < nSeg2; i ++) {
			int size = s2->getSegments()[i].getFaces().size();
			RI += CN2(size, nFaces);
		}
		for(int i = 0; i < nSeg1; i ++) {
			for(int j = 0; j < nSeg2; j ++) {
				int size = n[i][j];
				RI -= CN2(size, nFaces)*2;
			}
		}
		RI /= ((nFaces-1)/2);
		
		// cleaning
		for(int i = 0; i < nSeg1; i ++) {
			delete n[i];
		}	
		delete[] n;
	}
	
	Entry_RI* e = new Entry_RI;
	e->RI = RI;

	return e;
}

#endif