// CMSC 341 - Fall 2024 - Project 3
#ifndef MQUEUE_H
#define MQUEUE_H

#include <stdexcept>
#include <iostream>
#include <string>
using namespace std;
using std::out_of_range;
class Grader;   // forward declaration (for grading purposes)
class Tester;   //forward declaration
class MQueue;   //forward declaration
class Order;    //forward declaration
// Constant parameters, min and max values
const int MINZERO = 0;
const int MINONE = 1;
const int MIN1000 = 1000;
const int MAX12 = 12;
const int MAX50 = 50;
const int MAX100 = 100;
const int MAX200 = 200;
const int MAX10000 = 10000;
#define DEFAULTCUSTOMER "NONAME"

enum HEAPTYPE {MINHEAP, MAXHEAP};
enum STRUCTURE {SKEW, LEFTIST};

// Priority function pointer type
typedef int (*prifn_t)(const Order&);  

//
// order class
//
class Order {
  public:
    friend class Grader; // for grading purposes
    friend class Tester; // for testing purposes
    friend class MQueue;
    Order(){
        m_customer=DEFAULTCUSTOMER; m_FIFO=0; m_processTime=0;
        m_dueTime=0; m_slackTime=0; m_material=0;
        m_importance=0; m_workForce=0; m_quantity=0;
    }
    Order(string customer, int FIFO, int process, int due, int slack,
            int material, int importance, int workers, int quantity) {
        m_customer=customer; m_FIFO=FIFO; m_processTime=process;
        m_dueTime=due; m_slackTime=slack; m_material=material;
        m_importance=importance; m_workForce=workers; m_quantity=quantity;
    }

    string getCustomer() const {return m_customer;}
    
    int getFIFO() const {return m_FIFO;}
    int getProcessTime() const {return m_processTime;}
    int getDueTime() const {return m_dueTime;}
    int getSlackTime() const {return m_slackTime;}

    int getMaterial() const {return m_material;}
    int getImportance() const {return m_importance;}
    int getWorkForce() const {return m_workForce;}
    int getQuantity() const{return m_quantity;}

    // Overloaded insertion operator
    friend ostream& operator<<(ostream& sout, const Order& order);
  private:
    string m_customer;//customer name

    // m_FIFO is an integer indicating in what order customers placed their orders
    // it takes a value between 1 and 50, at any time the plant only accepts 50 orders at max
    // therefore this value cannot go beyond 50
    // a value of 1 means this was the first customer who placed the order
    // a value of 50 means this was the 50th customer who placed the order
    int m_FIFO;// (ordinal position) = 1(higher) - 50(lower)
    int m_processTime;// (month) = 1(higher) - 12(lower)
    int m_dueTime;// (month) = 1(higher) - 12(lower)
    int m_slackTime;// (month) = 1(higher) - 12(lower)

    int m_material;//material availability, (percentage) = 100(higher) - 0(lower)
    int m_importance;// customer importance (percentage) = 100(higher) - 1(lower)
    int m_workForce;// work force availability, (person count) = 200(higher) - 100(lower)
    int m_quantity;//quantity of the order, (unit count) = 10000(higher) - 1000(lower)
};

class Node {
  public:
    friend class Grader; // for grading purposes
    friend class Tester; // for testing purposes
    friend class MQueue;
    Node(Order order) {  
        m_order = order;
        m_right = nullptr;
        m_left = nullptr;
        m_npl = 0;
    }
    void setNPL(int npl) {m_npl = npl;}
    int getNPL() const {return m_npl;}
    Order getOrder() const {return m_order;}
    // Overloaded insertion operator
    friend ostream& operator<<(ostream& sout, const Node& node);
  private:
    Order m_order;   // order information
    Node * m_right;  // right child
    Node * m_left;   // left child
    int m_npl;       // null path length for leftist heap
};

class MQueue {
public:
    // stores the skew/leftist heap, minheap/maxheap
    friend class Grader; // for grading purposes
    friend class Tester; // for testing purposes
    MQueue(){}
    MQueue(prifn_t priFn, HEAPTYPE heapType, STRUCTURE structure);
    ~MQueue();
    void clear();
    MQueue(const MQueue& rhs);
    MQueue& operator=(const MQueue& rhs);
    bool insertOrder(const Order& input);
    Order getNextOrder();
    void mergeWithQueue(MQueue& rhs);
    int numOrders() const;
    // The following function prints the queue using preorder traversal.  
    // Although the first order printed should have the highest priority, 
    // the remaining orders will not necessarily be in priority order.
    void printOrderQueue() const;
    prifn_t getPriorityFn() const;
    // Set a new priority function. Must rebuild the heap!!!
    void setPriorityFn(prifn_t priFn, HEAPTYPE heapType);
    HEAPTYPE getHeapType() const;
    STRUCTURE getStructure() const;
    // Set a new data structure (skew/leftist). Must rebuild the heap!!!
    void setStructure(STRUCTURE structure);
    void dump() const; // For debugging purposes

private:
    Node * m_heap;          // Pointer to root of skew heap
    int m_size;             // Current size of the heap
    prifn_t m_priorFunc;    // Function to compute priority
    HEAPTYPE m_heapType;    // MINHEAP or MAXHEAP
    STRUCTURE m_structure;  // skew heap or leftist heap

    void dump(Node *pos) const; // helper function for dump

    /******************************************
     * Private function declarations go here! *
     ******************************************/
    void clear(Node* root);
    Node* copy(Node* rhs);
    Node* merge(Node* p1, Node* p2);
    bool leftHeavy(Node* root);
    void updateNPL(Node* root);
    void printOrderQueue(Node* root) const;
};

#endif