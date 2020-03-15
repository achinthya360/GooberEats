#include "provided.h"
#include <list>
#include <set>
#include <stack>
#include <utility>
#include <tuple>
#include "ExpandableHashMap.h"
using namespace std;

class PointToPointRouterImpl
{
public:
    PointToPointRouterImpl(const StreetMap* sm);
    ~PointToPointRouterImpl();
    DeliveryResult generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const;
    
private:
    const StreetMap* m_sm;
    
    struct coordDeets{
        coordDeets(){
            
        }
        
        coordDeets(GeoCoord parent, double g, double h){
            m_parent = parent;
            m_g = g;
            m_h = h;
        }
        
        GeoCoord m_parent;
        double m_g;
        double m_h;
    };
    
    void tracePath(GeoCoord start, GeoCoord end, ExpandableHashMap<GeoCoord, coordDeets>* coordDetailsPtr, list<StreetSegment>& route, double& totalDistanceTravelled) const;
};

PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
{
    m_sm = sm;
}

PointToPointRouterImpl::~PointToPointRouterImpl()
{
}

DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    if(start == end){   // 0 length path because the user is already at the destination
        route.clear();
        totalDistanceTravelled = 0;
        return DELIVERY_SUCCESS;
    }
    
    vector<StreetSegment> nextSegs;
    if(!(m_sm->getSegmentsThatStartWith(end, nextSegs) && m_sm->getSegmentsThatStartWith(start, nextSegs))){   // either start or end is not in the loaded map data
        //cerr << "Bad Coordinates!" << endl;
        return BAD_COORD;
    }
    
    // run A* algorithm if the start and end are valid routing points
    
    set<pair<double, GeoCoord>> openList;
    ExpandableHashMap<GeoCoord, bool> closedList;
    ExpandableHashMap<GeoCoord, coordDeets> coordDetails;
    
    // add starting node to open list
    openList.insert(pair<double, GeoCoord>(0, start));
    coordDetails.associate(start, coordDeets(start, 0, distanceEarthMiles(start, end)));
    
    // set this flag to false since destination not reached
    bool foundDest = false;
    
    while(!openList.empty())
    {
        pair<double, GeoCoord> p = *openList.begin();
        
        openList.erase(openList.begin());
        closedList.associate(p.second, true);
        vector<StreetSegment> successors;
        m_sm->getSegmentsThatStartWith(p.second, successors);
        
        // iterate through all adjacent GeoCoords of the current GeoCoord
        for(int i = 0; i < successors.size(); i++){
            // get current GeoCoord
            GeoCoord curNode = successors[i].end;
            
            // if the current GeoCoord is the destination
            if(curNode == end){
                foundDest = true;
                coordDeets endDeets(p.second, 0, 0);
                coordDetails.associate(curNode, endDeets);
                
                // call a path tracing function that changes the route list and totalDistanceTravelled value by retracing the path
                ExpandableHashMap<GeoCoord, coordDeets>* coordDetailsPtr = &coordDetails;
                tracePath(start, end, coordDetailsPtr, route, totalDistanceTravelled);
                
                //cerr << "Path completed successfully!" << endl;
                return DELIVERY_SUCCESS;
            }
            
            // if successor is already on closed list, ignore it
            // else, do the following
            else if(!closedList.find(curNode)){
                //double g = coordDetails.find(p.second)->m_g + 1;
                double g = distanceEarthMiles(p.second, curNode) + coordDetails.find(p.second)->m_g;
                double h = distanceEarthMiles(curNode, end);
                double f = g + h;
                
                coordDeets* curDetails = coordDetails.find(curNode);
                // if the node isn't on the open list or the current path is better than calculated before, put the curNode on the open list, set its parent to the root it came from and update its g and h in coordDeetails
                if(!curDetails || (curDetails->m_g + curDetails->m_h) > f){
                    openList.insert(pair<double, GeoCoord>(f, curNode));
                    coordDeets cd(p.second, g, h);
                    coordDetails.associate(curNode, cd);
                }
            }
        }
    }
    
    //cerr << "NO ROUTE!" << endl;
    return NO_ROUTE;  // Delete this line and implement this function correctly
}

void PointToPointRouterImpl::tracePath(GeoCoord start, GeoCoord end, ExpandableHashMap<GeoCoord, coordDeets>* coordDetailsPtr, list<StreetSegment>& route, double& totalDistanceTravelled) const{
    
    totalDistanceTravelled = 0;
    GeoCoord parent = coordDetailsPtr->find(end)->m_parent;
    stack<StreetSegment> backwardsPath;
    
    do{
        vector<StreetSegment> segs;
        m_sm->getSegmentsThatStartWith(parent, segs);
        StreetSegment curSeg;
        for(int i = 0; i < segs.size(); i++)
        {
            //cerr << curSeg.name << endl;
            if(segs[i].end == end){
                curSeg = segs[i];
                break;
            }
        }
        backwardsPath.push(curSeg);
        totalDistanceTravelled += distanceEarthMiles(curSeg.start, curSeg.end);
        end = parent;
        parent = coordDetailsPtr->find(end)->m_parent;
    }while(end != start);
    
    //cerr << "Number of street segments: " << backwardsPath.size() << endl;
    
    while(!backwardsPath.empty()){
        //cerr << "Street: " << backwardsPath.top().name << endl;
        route.push_back(backwardsPath.top());
        backwardsPath.pop();
    }
}

//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
    m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
    delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}
