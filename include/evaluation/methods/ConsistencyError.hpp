#ifndef CONSISTENCY_ERROR_HPP
#define CONSISTENCY_ERROR_HPP

#include "evaluation/Entry.hpp"
#include "evaluation/Segmentations.hpp"

/**************************************************************************************************
 **************************************************************************************************
 * EvalMethod 1 : Consistency Error ***************************************************************
 **************************************************************************************************
 **************************************************************************************************/

/*
 * Evaluate both global and local consistency error (GCE & LCE)
 * see Martin et. al in ICCV 01'
 */
struct Entry_CE* EvaluateConsistencyError(Segmentation *s1, Segmentation *s2)
{
  int i, j;
  double GCE = 0, LCE = 0, GCEa = 0, LCEa = 0;

  // Number of segments in s1 and s2
  int nSeg1 = s1->getSegments().size();
  int nSeg2 = s2->getSegments().size();

  // Number of faces in segments(should be the same for s1 and s2)
  int nFaces = s1->getMesh()->getMeshTopology().faceSize();

  // only compute under non-trial cases, otherwise, consistency errors are just 0
  if (nSeg1 != 1 && nSeg2 != 1 && nSeg1 != nFaces && nSeg2 != nFaces)
  {
    // Allocate metrics to store the difference between sets of segments
    // numFaces based
    double *SetDifference12 = new double[nSeg1 * nSeg2]; // s1 sets over s2 sets
    double *SetDifference21 = new double[nSeg2 * nSeg1]; // s2 sets over s1 sets
    double *Intersection = new double[nSeg1 * nSeg2];
    // area based
    double *AreaDifference12 = new double[nSeg1 * nSeg2]; // s1 areas over s2 areas
    double *AreaDifference21 = new double[nSeg2 * nSeg1]; // s2 areas over s1 areas
    double *AreaIntersection = new double[nSeg1 * nSeg2];
    for (i = 0; i < nSeg1 * nSeg2; i++)
    {
      // numFaces based
      SetDifference12[i] = 0;
      Intersection[i] = 0;
      SetDifference21[i] = 0;
      // area based
      AreaDifference12[i] = 0;
      AreaIntersection[i] = 0;
      AreaDifference21[i] = 0;
    }


    // get intersections
    for (FaceId face(0); face < s1->getMesh()->getMeshTopology().faceSize(); ++face)
    {
      int a = s1->getMesh()->getFaceCluster(face);
      int b = s2->getMesh()->getFaceCluster(face);
      Intersection[a * nSeg2 + b] += 1;
      AreaIntersection[a * nSeg2 + b] += s1->getMesh()->getMesh().area(face);
    }


    // get normalized set/area differences
    // faces in segments[i] does not necessarily have the segment id i
    double *nSegFaces1 = new double[nSeg1];
    double *nSegFaces2 = new double[nSeg2];
    double *areaSeg1 = new double[nSeg1];
    double *areaSeg2 = new double[nSeg2];

    for (i = 0; i < nSeg1; i++)
    {
      FaceId f = s1->getSegments()[i].getFaces()[0];
      int segId = s1->getMesh()->getFaceCluster(f);
      nSegFaces1[segId] = s1->getSegments()[i].getFaces().size();
      areaSeg1[segId] = s1->getSegments()[i].getArea();
    }
    for (i = 0; i < nSeg2; i++)
    {
      FaceId f = s2->getSegments()[i].getFaces()[0];
      int segId = s2->getMesh()->getFaceCluster(f);
      nSegFaces2[segId] = s2->getSegments()[i].getFaces().size();
      areaSeg2[segId] = s2->getSegments()[i].getArea();
    }
    

    for (i = 0; i < nSeg1; i++)
    {
      for (j = 0; j < nSeg2; j++)
      {
        SetDifference12[i * nSeg2 + j] = 1 - Intersection[i * nSeg2 + j] / nSegFaces1[i];
        SetDifference21[j * nSeg1 + i] = 1 - Intersection[i * nSeg2 + j] / nSegFaces2[j];
        AreaDifference12[i * nSeg2 + j] = 1 - AreaIntersection[i * nSeg2 + j] / areaSeg1[i];
        AreaDifference21[j * nSeg1 + i] = 1 - AreaIntersection[i * nSeg2 + j] / areaSeg2[j];
      }
    }

    // get GCE/LCE/GCEa/LCEa
    double sumE12 = 0, sumE21 = 0, sumMin = 0;
    double sumEa12 = 0, sumEa21 = 0, sumMina = 0;
    for (i = 0; i < nSeg1; i++)
    {
      for (j = 0; j < nSeg2; j++)
      {
        int frequency = int(Intersection[i * nSeg2 + j]);
        sumE12 += SetDifference12[i * nSeg2 + j] * frequency;
        sumE21 += SetDifference21[j * nSeg1 + i] * frequency;
        sumMin += std::min(SetDifference12[i * nSeg2 + j], SetDifference21[j * nSeg1 + i]) * frequency;
        sumEa12 += AreaDifference12[i * nSeg2 + j] * frequency;
        sumEa21 += AreaDifference21[j * nSeg1 + i] * frequency;
        sumMina += std::min(AreaDifference12[i * nSeg2 + j], AreaDifference21[j * nSeg1 + i]) * frequency;
      }
    }
    // numFaces based
    GCE = std::min(sumE12, sumE21) / nFaces;
    LCE = sumMin / nFaces;
    // area based
    GCEa = std::min(sumEa12, sumEa21) / nFaces;
    LCEa = sumMina / nFaces;

    // Release memories
    delete[] SetDifference12;
    delete[] SetDifference21;
    delete[] Intersection;
    delete[] AreaDifference12;
    delete[] AreaDifference21;
    delete[] AreaIntersection;
    delete[] nSegFaces1;
    delete[] nSegFaces2;
    delete[] areaSeg1;
    delete[] areaSeg2;
  }

  // return the booking entry
	Entry_CE *e = new Entry_CE;
	e->GCE = GCE;
	e->LCE = LCE;
	e->GCEa = GCEa;
	e->LCEa = LCEa;
	return e;
}

#endif