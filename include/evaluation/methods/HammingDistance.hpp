#ifndef HAMMING_DISTANCE_HPP
#define HAMMING_DISTANCE_HPP

#include "evaluation/Segmentations.hpp"
#include "evaluation/Entry.hpp"

/**************************************************************************************************
 **************************************************************************************************
 * EvalMethod 2 : Hamming Distance ****************************************************************
 **************************************************************************************************
 **************************************************************************************************/

/* 
 * Evaluate Hamming Distance, Huang et. al, ICIP 95'
 */
void EvaluateHammingDistance(Segmentation* s1, Segmentation* s2){
	
	// Variables
	int i, j;
  // Number of segments in s1 and s2
  int nSeg1 = s1->getSegments().size();
  int nSeg2 = s2->getSegments().size();

	// Total area, should be the same for s1 and s2
	double area = s1->getArea();
	
	// Keep the intersetion areas
	double **intersection = new double*[nSeg1];
	for(i = 0; i < nSeg1; i ++){
		intersection[i] = new double[nSeg2];
		for(int j = 0; j < nSeg2; j ++) {
			intersection[i][j] = 0;
		}
	}
  for (FaceId face(0); face < s1->getMesh()->getMeshTopology().faceSize(); ++face)
  {
    int i= s1->getMesh()->getFaceCluster(face);
    int j = s2->getMesh()->getFaceCluster(face);
		intersection[i][j] += s1->getMesh()->getMesh().area(face);
	}
	
	// Find the best matches
	int *matchForS1 = new int[nSeg1];
	int *matchForS2 = new int[nSeg2];
	for(i = 0; i < nSeg1; i ++){
		int index = 0;
		double largest = -10000;
		for(j = 0; j < nSeg2; j ++){
			if(intersection[i][j] > largest){
				largest = intersection[i][j];
				index =j;
			}
		}
		matchForS1[i] = index;
	}
	for(j = 0; j < nSeg2; j ++){
		int index = 0;
		double largest = -10000;
		for(i = 0; i < nSeg1; i ++){
			if(intersection[i][j] > largest){
				largest = intersection[i][j];
				index = i;
			}
		}
		matchForS2[j] = index;
	}
	
	// Compute directional Hamming distance
	// dh12 = sum(areaSeg2[j]-intersection[matchForS2[j]][j])
	//      = sum(areaSeg2[j]) - sum(intersection[matchForS2[j]][j])
	//      = area - sum(intersection[matchForS2[j]][j]
	double dh12 = area, dh21 = area;
	for(j = 0; j < nSeg2; j ++){
		dh12 -= intersection[matchForS2[j]][j];
	}
	for(i = 0; i < nSeg1; i ++){
		dh21 -= intersection[i][matchForS1[i]];
	}
	
  printf("Evaluation of Hamming Distance ...\n");
  printf("  HammingDistance  = %.2f \n", (dh12 + dh21) / (2 * area));
  printf("  MissingRate  = %.2f \n", dh12 / area);
  printf("  FalseAlarmRate = %.2f \n", dh21 / area);
  fflush(stdout);
	
	// Release memories
	for(i = 0; i < nSeg1; i ++) 
		delete []intersection[i];
	delete []intersection;
	delete []matchForS1;
	delete []matchForS2;
}

#endif // HAMMING_DISTANCE_HPP