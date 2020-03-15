// ExpandableHashMap.h

// Skeleton for the ExpandableHashMap class template.  You must implement the first six
// member functions.
#include <list>
#include <vector>

template<typename KeyType, typename ValueType>
class ExpandableHashMap
{
public:
    ExpandableHashMap(double maximumLoadFactor = 0.5);
    ~ExpandableHashMap();
    void reset();
    int size() const;
    void associate(const KeyType& key, const ValueType& value);

      // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;

      // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
    }
    
      // C++11 syntax for preventing copying and assignment
    ExpandableHashMap(const ExpandableHashMap&) = delete;
    ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;

private:
    struct Node{
        KeyType k;
        ValueType v;
    };
    
    int m_numBuckets;
    double m_maxLoadFactor;
    int m_numAssociations;
    std::vector<std::list<Node*>> m_map;
    
    unsigned int getBucketNumber(const KeyType& key) const
    {
        unsigned int hasher(const KeyType& k); // prototype
        unsigned int h = hasher(key);
        return h % m_numBuckets;
    }
    
    double calculateLoad(){
        return (static_cast<double>(m_numAssociations) + 1)/ m_numBuckets;
    }
};

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::ExpandableHashMap(double maximumLoadFactor)
{
    m_numBuckets = 8;
    m_numAssociations = 0;
    m_maxLoadFactor = maximumLoadFactor;
    m_map.resize(m_numBuckets);   // sets vector to 8 buckets
}

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::~ExpandableHashMap()
{
    m_map.clear();
    m_numAssociations = 0;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset()
{
    m_map.clear();
    m_numAssociations = 0;
    m_numBuckets = 8;
}

template<typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::size() const
{
    return m_numAssociations;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
    if(calculateLoad() > m_maxLoadFactor){   // if rehashing required, rehash the map
        m_numBuckets *= 2;
        std::vector<std::list<Node*>> tempMap;
        tempMap.resize(m_numBuckets);
        for(int bucket = 0; bucket < m_numBuckets/2; bucket++){
            typename std::list<Node*>::iterator it;
            for(it = m_map[bucket].begin(); it != m_map[bucket].end(); it++){
                unsigned int newBuckNum = getBucketNumber((*it)->k); 
                tempMap[newBuckNum].push_back(*it);
            }
        }
        m_map.clear();
        m_map = tempMap;
    }
    unsigned int buckNum = getBucketNumber(key);
    // insert into or update map
    if(find(key) != nullptr) {
        typename std::list<Node*>::iterator it;
        for(it = m_map[buckNum].begin(); it != m_map[buckNum].end(); it++){
            if((*it)->k == key){  // if key already in map, update
                (*it)->v = value;
                return;
            }
        }
    }
    // if key not found in map, insert new key-value pair
    Node* newNode = new Node;
    newNode->k = key;
    newNode->v = value;
    m_map[buckNum].push_back(newNode);
    m_numAssociations++;
}

template<typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const
{
    std::list<Node*> list;
    typename std::list<Node*>::iterator it;
    unsigned int buckNum = getBucketNumber(key);
    list = m_map[buckNum];
    it = list.begin();
    for(; it != list.end(); it++){
        Node* thisNode = *it;
        if(thisNode->k == key){
            return &(thisNode->v);
        }
    }
    return nullptr;  // Delete this line and implement this function correctly
}

