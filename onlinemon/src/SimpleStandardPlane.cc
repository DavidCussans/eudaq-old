/*
 * SimpleStandardPlane.cxx
 *
 *  Created on: Jun 9, 2011
 *      Author: stanitz
 */


#include <string>
#include <vector>
#include "include/SimpleStandardPlane.hh"

void SimpleStandardPlane::addHit(SimpleStandardHit oneHit) {//oneHit.reduce(_binsX, _binsY); //also fills the appropriate sections
	_hits.push_back(oneHit);
	if  (_name.compare("MIMOSA26")==0)
	{
		int section =oneHit.getX()/288;
		if ((section <0)||(section >3))
		{
			std::cout << "Error Section invalid: " << section << " "<< oneHit.getX()<< " "<< oneHit.getY()<<std::endl;
			_badhits.push_back(oneHit);

		}
		else
		{
			_section_hits[section].push_back(oneHit);
		}


	}
}

void SimpleStandardPlane::reducePixels(const int reduceX, const int reduceY) {

	_binsX = reduceX;
	_binsY = reduceY;
	//std::cout << "Reducing " << reduce << " -> " << _reduce <<" " << _maxX << " -> " << _binsX << " " << _maxY << " -> " << _binsY << std::endl;

}


void SimpleStandardPlane::doClustering() {
	int nClusters = 0;
	std::vector<int> clusterNumber;
	const int NOCLUSTER = -1000;
	const unsigned int minXDistance = 1;
	const unsigned int minYDistance = 1;
	const unsigned int npixels_hit=_hits.size();
	clusterNumber.assign(npixels_hit, NOCLUSTER);
	//std::cout << "Before big loop" << std::endl;
	//std::cout << "Name: " << _name << std::endl;

	// which planes to cluster, reject planes of Type Fortis
	if (strcmp(_name.c_str(), "FORTIS") == 0)
	{
		return;
	}


	if (npixels_hit > 1 && npixels_hit < 1000 )
	{
		for (unsigned int aPixel = 0; aPixel < npixels_hit; aPixel++)
		{
			SimpleStandardHit aPix = _hits.at(aPixel);
			for (unsigned int bPixel = aPixel+1; bPixel < npixels_hit; bPixel++)
			{
				//std::cout << aPixel << " " << bPixel << " " << _hits.size() << std::endl;
				SimpleStandardHit bPix = _hits.at(bPixel);
				unsigned int xDist = abs(aPix.getX() - bPix.getX());
				unsigned int yDist = abs(aPix.getY() - bPix.getY());

				if ( ( xDist <= minXDistance) && (yDist <= minYDistance)  )
				{ //this means they are neighbors in x-direction && / this means they are neighbors in y-direction
					if ( (clusterNumber.at(aPixel) == NOCLUSTER) && clusterNumber.at(bPixel) == NOCLUSTER)
					{ // none of these pixels have been assigned to a cluster
						++nClusters;
						clusterNumber.at(aPixel) = nClusters;
						clusterNumber.at(bPixel) = nClusters;
					}
					else if ( (clusterNumber.at(aPixel) == NOCLUSTER) && (clusterNumber.at(bPixel) != NOCLUSTER))
					{ // b was assigned already, a not
						clusterNumber.at(aPixel) = clusterNumber.at(bPixel);
					}
					else if ( (clusterNumber.at(aPixel) != NOCLUSTER) && (clusterNumber.at(bPixel) == NOCLUSTER))
					{ // a was assigned already, b not
						clusterNumber.at(bPixel) = clusterNumber.at(aPixel);
					}
					else
					{ //both pixels have a cluster number already
						int min = std::min(clusterNumber.at(aPixel), clusterNumber.at(bPixel));
						clusterNumber.at(aPixel) = min;
						clusterNumber.at(bPixel) = min;
					}
				}
				else
				{ // these pixels are not neighbored
					if ( clusterNumber.at(aPixel) == NOCLUSTER )
					{
						++nClusters;
						clusterNumber.at(aPixel) = nClusters;
					}
					if ( clusterNumber.at(bPixel) == NOCLUSTER )
					{
						++nClusters;
						clusterNumber.at(bPixel) = nClusters;
					}
				}
			} // inner for loop
		} //outer for loop
	}
	else
	{ //You can't use the clustering algorithm with only one pixel
		if (_hits.size() == 1) clusterNumber.at(0) = 1;
	}

	//std::cout << "After big loop" << std::endl;
	std::set<int> clusterSet;
	for (unsigned int i=0; i<_hits.size();++i) {
		if ( clusterNumber.at(i) == NOCLUSTER) {
			//std::cerr << "Cluster with ID -1 in " << i << std::endl;
		} else {
			clusterSet.insert(clusterNumber.at(i));
		}
	}
	nClusters = clusterSet.size();

	std::set<int>::iterator it;
	for (it=clusterSet.begin();it!=clusterSet.end();it++)
	{
		SimpleStandardCluster cluster;
		for (unsigned int i=0;i< _hits.size();i++)
		{
			if(clusterNumber.at(i)== *it)
			{ //Put only these pixels in that ClusterCollection that belong to that cluster
				cluster.addPixel(_hits.at(i));
			}
		}
		_clusters.push_back(cluster);
		//std::cout << "Cluster at: " << cluster.getX() << std::endl;
	}
	// if we have a mimosa, we need to fill the section information
	if (_name.compare("MIMOSA26")==0)
	{
		for (unsigned int mycluster=0; mycluster<_clusters.size(); mycluster++)
		{
			unsigned int cluster_section=_clusters[mycluster].getX()/288;
			if (cluster_section<4) //FIXME
			{
				_section_clusters[cluster_section].push_back(_clusters[mycluster]);
			}
		}
	}
}