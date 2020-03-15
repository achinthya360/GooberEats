#include "provided.h"
#include <string>
#include <vector>
#include <functional>
#include "ExpandableHashMap.h"
#include <iostream>
#include <fstream>
using namespace std;

unsigned int hasher(const GeoCoord& g)
{
    return std::hash<string>()(g.latitudeText + g.longitudeText);
}

unsigned int hasher(const string& g)
{
    return std::hash<string>()(g);
}

class StreetMapImpl
{
public:
    StreetMapImpl();
    ~StreetMapImpl();
    bool load(string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const;
    
private:
    ExpandableHashMap<GeoCoord, vector<StreetSegment>> m_map;
    void insertSeg(StreetSegment seg);
};

StreetMapImpl::StreetMapImpl()
{
}

StreetMapImpl::~StreetMapImpl()
{
}

bool StreetMapImpl::load(string mapFile)
{
    ifstream infile(mapFile);    // infile is a name of our choosing
    if ( ! infile )                // Did opening the file fail?
    {
        //cerr << "Error: Cannot open the file" << endl;
        return false;
    }
    
    string name;
    while(getline(infile, name)){
        
        int numSegments;
        //cerr << name << endl;
        string stringnum("");
        getline(infile, stringnum);
        if(stringnum == "")
            break;
        numSegments = stoi(stringnum);
        //cerr << numSegments << endl;
        
        for(int i = 0; i < numSegments; i++){
            string lat1, lon1, lat2, lon2;
            infile >> lat1;
            infile >> lon1;
            infile >> lat2;
            infile >> lon2;
            //cerr << lat1 << lon1 << lat2 << lon2 << endl;
        
            GeoCoord g1(lat1, lon1);
            GeoCoord g2(lat2, lon2);
            StreetSegment s(g1, g2, name);
            StreetSegment rs(g2, g1, name);
            
            insertSeg(s);
            insertSeg(rs);
        }
        infile.ignore(10000, '\n');
    }
    //cerr << m_map.size() << endl;
    return true;
}

bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
    const vector<StreetSegment>* segsPtr = m_map.find(gc);
    if(segsPtr){
        segs = *segsPtr;
        return true;
    }
    return false;
}

void StreetMapImpl::insertSeg(StreetSegment seg){
    vector<StreetSegment>* segments = m_map.find(seg.start);
     if(segments == nullptr){
         vector<StreetSegment> newsegments;
         newsegments.push_back(seg);
         m_map.associate(seg.start, newsegments);
     }
     else{
         segments->push_back(seg);
     }
}

//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap()
{
    m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap()
{
    delete m_impl;
}

bool StreetMap::load(string mapFile)
{
    return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
   return m_impl->getSegmentsThatStartWith(gc, segs);
}
