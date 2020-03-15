#include "provided.h"
#include <vector>
using namespace std;

class DeliveryPlannerImpl
{
public:
    DeliveryPlannerImpl(const StreetMap* sm);
    ~DeliveryPlannerImpl();
    DeliveryResult generateDeliveryPlan(
        const GeoCoord& depot,
        const vector<DeliveryRequest>& deliveries,
        vector<DeliveryCommand>& commands,
        double& totalDistanceTravelled) const;
private:
    PointToPointRouter m_router;
    DeliveryOptimizer  m_optimizer;
    
    string generateProceedCommand(StreetSegment seg) const;
    int generateTurnCommand(StreetSegment seg1, StreetSegment seg2) const;
};

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
: m_router(sm), m_optimizer(sm)
{
}

DeliveryPlannerImpl::~DeliveryPlannerImpl()
{
}

DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    if(deliveries.size() <= 0)
        return DELIVERY_SUCCESS;
    
    // optimize route
    vector<DeliveryRequest> betterDeliveries;
    for(int i = 0; i < deliveries.size(); i++){
        betterDeliveries.push_back(deliveries[i]);
    }

    double ocd, ncd;
    m_optimizer.optimizeDeliveryOrder(depot, betterDeliveries, ocd, ncd);
    DeliveryRequest returnToDepot("DEPOT", depot);
    betterDeliveries.push_back(returnToDepot);
    
    // plan route
    GeoCoord curCoord = depot;
    list<StreetSegment>::iterator curSeg, prevSeg;
    totalDistanceTravelled = 0;
    
    // go from point to point and generate the routes
    for(int i = 0; i < betterDeliveries.size(); i++){
        
        // temporary variables to store each route and travel distance given by PointToPointerRouter
        list<StreetSegment> route;
        double travelDist = 0;
        // generate a route from the current location to the next coordinate
        GeoCoord nextCoord = betterDeliveries[i].location;
        DeliveryResult result = m_router.generatePointToPointRoute(curCoord, betterDeliveries[i].location, route, travelDist);
        
        // add to the total distance travelled
        totalDistanceTravelled += travelDist;
        
        // make sure the path was routed successfully
        if(result != DELIVERY_SUCCESS)
            return result;
    
        curSeg = route.begin();
        
        // first generate a proceed command to the start of the route
        DeliveryCommand proceed;
        proceed.initAsProceedCommand(generateProceedCommand(*curSeg), curSeg->name, distanceEarthMiles(curSeg->start, curSeg->end));
        commands.push_back(proceed);
        prevSeg = curSeg;
        curSeg++;
        
        // iterate through the route and generate turns and proceed commands
        while(curSeg != route.end()){
            double curSegDistance = distanceEarthMiles(curSeg->start, curSeg->end);
            // proceed onto same street
            if(curSeg->name == prevSeg->name){
                commands[commands.size()-1].increaseDistance(curSegDistance);
            }
            else{
                int decision = generateTurnCommand(*prevSeg, *curSeg);
                if(decision == 1){ // left turn command
                    DeliveryCommand leftTurn;
                    leftTurn.initAsTurnCommand("left", curSeg->name);
                    commands.push_back(leftTurn);
                }
                else{ // right turn command
                    DeliveryCommand rightTurn;
                    rightTurn.initAsTurnCommand("right", curSeg->name);
                    commands.push_back(rightTurn);
                }
                // always has a proceed command following the turn even when no turn
                DeliveryCommand curProceed;
                curProceed.initAsProceedCommand(generateProceedCommand(*curSeg), curSeg->name, curSegDistance);
                commands.push_back(curProceed);
            }
            // move to next segment of route
            prevSeg = curSeg;
            curSeg++;
        }
        
        if(betterDeliveries[i].item == "DEPOT")  // when the last DeliveryRequest is back to the depot, the loop returns success
            return DELIVERY_SUCCESS;
        
        // generate deliver command when done
        DeliveryCommand delivered;
        delivered.initAsDeliverCommand(betterDeliveries[i].item);
        commands.push_back(delivered);
        
        // update current location
        curCoord = nextCoord;
    }
    return DELIVERY_SUCCESS;
}

string DeliveryPlannerImpl::generateProceedCommand(StreetSegment seg) const
{
    double angle = angleOfLine(seg);
    if(angle < 22.5)
        return "east";
    if(angle < 67.5)
        return "northeast";
    if(angle < 112.5)
        return "north";
    if(angle < 157.5)
        return "northwest";
    if(angle < 202.5)
        return "west";
    if(angle < 247.5)
        return "southwest";
    if(angle < 292.5)
        return "south";
    if(angle < 337.5)
        return "southeast";
    else
        return "east";
}

int DeliveryPlannerImpl::generateTurnCommand(StreetSegment seg1, StreetSegment seg2) const
{
    double angle = angleBetween2Lines(seg1, seg2);
    if(angle < 1 || angle > 359)
        return 0; // proceed command
    if(angle >= 1 && angle < 180)
        return 1; // left command
    else
        return 2; // right command
}

//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
{
    m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner()
{
    delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}
