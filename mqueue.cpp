// CMSC 341 - Fall 2024 - Project 3
#include "mqueue.h"
MQueue::MQueue(prifn_t priFn, HEAPTYPE heapType, STRUCTURE structure){
  //Set m_heap to nullptr
  m_heap = nullptr;
  //Set m_size to 0
  m_size = 0;
  //Set priority function
  m_priorFunc = priFn;
  //Set heap type
  m_heapType = heapType;
  //Set structure type
  m_structure = structure;
}

MQueue::~MQueue(){
  //Call the clear function to delete the heap
  clear();
}
//Helper function for clear
void MQueue::clear(Node* root) {
  if (root != nullptr) {
    //Recursively clear the left and right nodes
    clear(root->m_left);
    clear(root->m_right);
    //Delete the node
    delete root;
  }
}
void MQueue::clear() {
  //Check if the heap is empty
  if (m_heap != nullptr) {
    //Call the helper function to clear the heap
    clear(m_heap);
  }
  //Set the size to 0
  m_size = 0;
  //Set the heap to nullptr
  m_heap = nullptr;
  //Set the priority function to nullptr
  m_priorFunc = nullptr;
  //Set the heap type to MINHEAP
  m_heapType = MINHEAP;
  //Set the structure type to LEFTIST
  m_structure = LEFTIST;
}
//Helper function to copy data
Node* MQueue::copy(Node* rhs){
  //Base case
  if (rhs == nullptr) {
    return nullptr;
  }
  //Copying rhs order
  Order order(rhs->m_order.getCustomer(), rhs->m_order.getFIFO(), rhs->m_order.getProcessTime(), rhs->m_order.getDueTime(),
  rhs->m_order.getSlackTime(), rhs->m_order.getMaterial(), rhs->m_order.getImportance(), rhs->m_order.getWorkForce(), rhs->m_order.getQuantity());
  //Create a new node
  Node* newNode = new Node(order);
  //Recursively copy the left and right nodes
  newNode->m_left = copy(rhs->m_left);
  newNode->m_right = copy(rhs->m_right);
  //Check for leftist
  if (m_structure == LEFTIST) {
    //Set the npl
    newNode->m_npl = rhs->m_npl;
  }
  //Return the new node
  return newNode;
}
MQueue::MQueue(const MQueue& rhs){
  //Intialize all member variables
  m_size = rhs.m_size;
  m_priorFunc = rhs.m_priorFunc;
  m_heapType = rhs.m_heapType;
  m_structure = rhs.m_structure;
  //Call the copy helper function to copy data
  m_heap = copy(rhs.m_heap);
}

MQueue& MQueue::operator=(const MQueue& rhs) {
  //Check for self assignment
  if (this != &rhs) {
    if (m_heap != nullptr) {
      //Call the clear function to delete the heap
      clear();
    }
    //Copy the data to new object
    m_size = rhs.m_size;
    m_priorFunc = rhs.m_priorFunc;
    m_heapType = rhs.m_heapType;
    m_structure = rhs.m_structure;
    m_heap = copy(rhs.m_heap);
  }
  //Return the object
  return *this;
}
Node* MQueue::merge(Node* p1, Node* p2){
  //Base case
  if (p1 == nullptr) return p2;
  if (p2 == nullptr) return p1;

  //Check for max heap
  if (m_heapType == MAXHEAP) {
    //Check if the priority function of p1 is less than p2
    if (m_priorFunc(p1->m_order) < m_priorFunc(p2->m_order)) {
      //Swap the nodes
      Node* temp = p1;
      p1 = p2;
      p2 = temp;
    }
  } else {
    //Min heap
    //Check if the priority function of p1 is greater than p2
    if (m_priorFunc(p1->m_order) > m_priorFunc(p2->m_order)) {
      //Swap the nodes
      Node* temp = p1;
      p1 = p2;
      p2 = temp;
    }
  }
  //Check for leftist
  if (m_structure == LEFTIST) {
    //Recursively merge the left and right nodes
    p1->m_right = merge(p1->m_right, p2);
    //Update the npl
    updateNPL(p1);
    //Check that is leftHeavy
    if (leftHeavy(p1) == false) {
      //Swap the left and right nodes
      Node* temp = p1->m_left;
      p1->m_left = p1->m_right;
      p1->m_right = temp;
      //Set temp to null
      temp = nullptr;
    }
  } else if (m_structure == SKEW) {
    //Swap left and right node for skew
    Node* temp = p1->m_left;
    p1->m_left = p1->m_right;
    p1->m_right = temp;
    //Set temp to null
    temp = nullptr;
    //Recursively merge the left and right nodes
    p1->m_left = merge(p1->m_left, p2);
  
  }
  //Return the node
  return p1;
}
bool MQueue::leftHeavy(Node* root){
  //Default value
  int leftNPL = -1, rightNPL = -1;
  //Getting node's children NPL
  if (root->m_left != nullptr) {
    leftNPL = root->m_left->m_npl;
  }
  if (root->m_right != nullptr) {
    rightNPL = root->m_right->m_npl;
  }
  //Check that is left heavy
  if (rightNPL < leftNPL) {
    return true;
  }
  //Return false
  return false;
}
void MQueue::updateNPL(Node* root){
  //Default value
  int leftNPL = -1, rightNPL = -1;
  //Getting node's children NPL
  if (root->m_left != nullptr) {
    leftNPL = root->m_left->m_npl;
  }
  if (root->m_right != nullptr) {
    rightNPL = root->m_right->m_npl;
  }
  //Update the NPL
  if (leftNPL > rightNPL) {
    root->m_npl = leftNPL + 1;
  } else {
    root->m_npl = rightNPL + 1;
  }
}
void MQueue::mergeWithQueue(MQueue& rhs) {
    // Check for self merge
    if (this == &rhs) {
        throw domain_error("Cannot merge queue with itself");
    }

    // Check if structures, heap types, and priority functions match
    if (m_structure != rhs.m_structure || 
        m_heapType != rhs.m_heapType ||
        m_priorFunc != rhs.m_priorFunc) {
        throw domain_error("Incompatible queues");
    }

    // Merge the queues
    m_size += rhs.m_size;
    m_heap = merge(m_heap, rhs.m_heap);
    
    // Empty rhs
    rhs.m_heap = nullptr;
    rhs.m_size = 0;
}
bool MQueue::insertOrder(const Order& input) {
  //Check for valid priority 
  if (m_priorFunc(input) == 0) {
    return false;
  }
  //Make new node
  Node* newNode = new Node(input);
  //Call the merge function to insert the order
  m_heap = merge(m_heap, newNode);
  //Increment the size
  m_size++;
  //Return true
  return true;
}
int MQueue::numOrders() const{
  //Return numbers of orders
  return m_size;
}
  
prifn_t MQueue::getPriorityFn() const {
  //Return the priority function
  return m_priorFunc;
}
Order MQueue::getNextOrder() {
  //Throw exception if the heap is empty
  if (m_heap == nullptr) {
    throw out_of_range("Out of range: Empty heap");
  }
  //Store the m_heap
  Node* temp = m_heap;

  //Save the order of m_heap
  Order order = m_heap->m_order;

  //Merge the left and right nodes
  m_heap = merge(m_heap->m_left, m_heap->m_right);

  //Delete the m_heap
  delete temp;

  //Decrement the size
  m_size--;

  //Return the order
  return order;
}
void MQueue::setPriorityFn(prifn_t priFn, HEAPTYPE heapType) {
  //Set values
  m_priorFunc = priFn;
  m_heapType = heapType;
  //Create temporary queue pointer
  MQueue* tempQueue = new MQueue(priFn, heapType, m_structure);
  //Restructure the temporary heap
  while (m_heap != nullptr) {
    //Insert the order
    tempQueue->insertOrder(getNextOrder());
  }
  //Merge new heap
  m_heap = merge(m_heap, tempQueue->m_heap);
  m_heap = tempQueue->m_heap;
  //Take new size
  m_size = tempQueue->m_size;
  //Delete the temporary queue
  tempQueue->m_heap = nullptr;
  delete tempQueue;
  tempQueue = nullptr;
}                    

void MQueue::setStructure(STRUCTURE structure){
  if (m_structure != structure) {
    //Create temporary queue pointer
    MQueue* tempQueue = new MQueue(m_priorFunc, m_heapType, structure);
    //Restructure the temporary heap
    while (m_heap != nullptr) {
      //Insert the order
      tempQueue->insertOrder(getNextOrder());
    }
    //Merge new heap
    m_heap = merge(m_heap, tempQueue->m_heap);
    m_heap = tempQueue->m_heap;
    //Take new size
    m_size = tempQueue->m_size;
    //Delete the temporary queue
    tempQueue->m_heap = nullptr;
    delete tempQueue;
    tempQueue = nullptr;
  }
}
STRUCTURE MQueue::getStructure() const {
  //Return the structure type
  return m_structure;
}
HEAPTYPE MQueue::getHeapType() const{
  //Return the heap type
  return m_heapType;
}
void MQueue::printOrderQueue() const {
  if (m_heap == nullptr) {
    cout << "Empty heap.\n";
  } else {
    printOrderQueue(m_heap);
  }
}
void MQueue::printOrderQueue(Node* root) const {
  if (root != nullptr) {
    //Preorder traversal to print the order
    cout << root->m_order << endl;
    printOrderQueue(root->m_left);
    printOrderQueue(root->m_right);
  }
}
void MQueue::dump() const {
  if (m_size == 0) {
    cout << "Empty heap.\n" ;
  } else {
    dump(m_heap);
  }
  cout << endl;
}
void MQueue::dump(Node *pos) const {
  if ( pos != nullptr ) {
    cout << "(";
    dump(pos->m_left);
    if (m_structure == SKEW)
        cout << m_priorFunc(pos->m_order) << ":" << pos->m_order.m_customer;
    else
        cout << m_priorFunc(pos->m_order) << ":" << pos->m_order.m_customer << ":" << pos->m_npl;
    dump(pos->m_right);
    cout << ")";
  }
}

ostream& operator<<(ostream& sout, const Order& order) {
  sout  << "Customer: " << order.getCustomer()
        << ", importance: " << order.getImportance() 
        << ", quantity: " << order.getQuantity() 
        << ", order placement time: " << order.getFIFO();
  return sout;
}
ostream& operator<<(ostream& sout, const Node& node) {
  sout << node.m_order;
  return sout;
}