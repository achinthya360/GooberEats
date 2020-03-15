#include "provided.h"
#include <vector>
using namespace std;

class DeliveryOptimizerImpl
{
public:
    DeliveryOptimizerImpl(const StreetMap* sm);
    ~DeliveryOptimizerImpl();
    void optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const;
    
private:
    const StreetMap* m_sm;
    double calcCrowsDist(const GeoCoord& depot, vector<DeliveryRequest>& deliveries) const;
};

DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm)
{
    m_sm = sm;
}

DeliveryOptimizerImpl::~DeliveryOptimizerImpl()
{
}

void DeliveryOptimizerImpl::optimizeDeliveryOrder(
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const
{
    oldCrowDistance = 0; 
    newCrowDistance = 0;
    
    if(deliveries.size() <= 0)
        return;
    
    oldCrowDistance = calcCrowsDist(depot, deliveries);
    
    // put delivery location closest to depot in the first slot of deliveries array
    double closestToDepotDist = distanceEarthMiles(depot, deliveries[0].location);
    int closestToDepotPos = 0;
    for(int i = 1; i < deliveries.size(); i++){
        double distToDepot = distanceEarthMiles(depot, deliveries[i].location);
        if(distToDepot < closestToDepotDist){
            closestToDepotDist = distToDepot;
            closestToDepotPos = i;
        }
    }
    if(closestToDepotPos != 0)
        std::swap(deliveries[0], deliveries[closestToDepotPos]);
    
    
    // reorganize/optimize route based on crows distances between points
    for(int i = 0; i < deliveries.size() - 1; i++)
    {
        double closestDist = distanceEarthMiles(deliveries[i].location, deliveries[i+1].location);
        int closestPos = i + 1;
        for(int k = i + 2; k < deliveries.size(); k++)
        {
            double curDist = distanceEarthMiles(deliveries[i].location, deliveries[k].location);
            if(curDist < closestDist){
                closestDist= curDist;
                closestPos = k;
            }
        }
        if(closestPos != i + 1)
            std::swap(deliveries[i + 1], deliveries[closestPos]);
    }
    
    newCrowDistance = calcCrowsDist(depot, deliveries);
}

double DeliveryOptimizerImpl::calcCrowsDist(const GeoCoord& depot, vector<DeliveryRequest>& deliveries) const
{
    double crowDistance = distanceEarthMiles(depot, deliveries[0].location);
    for(int i = 0; i < deliveries.size() - 1; i++){
        crowDistance += distanceEarthMiles(deliveries[i].location, deliveries[i+1].location);
    }
    return crowDistance;
}

//******************** DeliveryOptimizer functions ****************************

// These functions simply delegate to DeliveryOptimizerImpl's functions.
// You probably don't want to change any of this code.

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm)
{
    m_impl = new DeliveryOptimizerImpl(sm);
}

DeliveryOptimizer::~DeliveryOptimizer()
{
    delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const
{
    return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}
