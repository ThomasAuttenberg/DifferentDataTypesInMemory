<h1 align="center">
«Collection of objects of arbitrary types in memory. Doubly linked circular list»
</h1>
<h2><b>Task</b></h2>
You need to develop an interface for structuring a collection of objects of different classes - an abstract base class called "object," for which you should provide virtual methods: loading an object from a text string, saving an object to a text string in dynamic memory, adding an object to a sequential binary file, reading an object from a sequential binary file, returning a unique class identifier, returning a pointer to the class name string, comparing two objects, "adding" (combining) two objects, creating a dynamic copy of an object.<br>Make the classes of stored objects derived from the abstract base class "object" (e.g., Float, String, and a class from a previous lab).<br>The collection is a data structure that stores pointers to objects of arbitrary types - objects of the base class. The program should provide functionalities such as viewing the collection, adding objects of selected types, editing and deleting objects, combining and comparing objects with the same types, and saving and loading objects from a text (or binary) file.<br>The program should implement the actions mentioned above and should be tested with simple types (e.g., Int or String). The testing program should include a menu to choose these operations.<br><br>Type of data structure:<b> Doubly linked circular list </b><br><br>Additional feature: Add a nested class to the collection that implements iterator functions 
<br><br><br>

<h2>Solution text:</h2>
<h3><b>Structural Description</b></h3><br>
The collection itself - a doubly-linked circular list - is designed in the following manner:
The class is a template. The template parameter specifies what type the collection should work with.<br>
When designing the class, the goal was to achieve the maximum external compatibility with data structures presented in the Standard Template Library (STL). That is, the class should include a set of standard functions (insert, erase, etc.), constant and non-constant iterators that provide bidirectional navigation through the collection according to the data structure. The collection stores values in a "Node" structure, created dynamically, containing the stored value and pointers to other nodes. According to the chosen theme (doubly-linked circular list), sequentially arranged nodes are connected by pointers. The first and last nodes also point to each other. The collection itself stores a pointer to the first node. The corresponding illustration is provided in Figure 1.<br><br>
<div align = center>
<img src=https://i.imgur.com/2JdVLEz.png alt="no img" />
</div>
<p align = center>Figure 1 - Collection schema.</p>

Since the list is circular, it deviates from the functional feature of STL collections. The end iterator, which should point to the element after the last one, will point to the first element, whereas in other collections, the end iterator should point to a nonexistent element. Thanks to this feature, standard collections make it easier to iterate through the entire collection. In our case, the beginning iterator is equal to the end iterator. Thus, traversing the collection should work according to the following algorithm:<br>
<pre>
&nbsp;&nbsp;&nbsp;&nbsp;if (!empty()) {<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;iterator it = begin();<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;do {<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;...;<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;it++;<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;} while (it != begin());<br>
&nbsp;&nbsp;&nbsp;&nbsp;}</pre>
Where begin() is a function that returns an iterator pointing to the first element of the list, and empty() is a method that answers whether the container is empty. To make it more convenient, the collection should have a function that provides an interface for easier traversal of the collection. The composed function will have the following signature:<br>
<pre>auto foreach(std::function< void(T&)>)</pre>
where T is a template parameter.<br>
In other words, the method takes a function as input, which performs an action on the value. The implementation uses the traversal algorithm mentioned above, calling the function received as an argument within the loop.<br>

According to the assignment, it is necessary to have an Object class. The class will be abstract, so the collection should store its descendants, implementing the behavior specified by pure virtual functions as pointers to Object objects.<br>
The class includes the functionality specified in the assignment from pure virtual functions determined by the derived classes. For the convenience of designing descendants, it was decided to define several additional implementations that achieve the task of storing data of any type in Object objects. Thus, Object encapsulates data storage, while descendants can simply use the provided interface, making it easy to create wrappers for any data types.<br>
To perform this task, Object must correctly manage its memory. To do this, we define that the class should contain a pointer of type void* to the allocated memory area and be able to free it correctly: objects that may be contained in it may require a call to their destructor. Given that derived classes will not manage data on their own, it was decided to implement a correct initialization algorithm in the constructor, which will be manually called by the derived classes during their own initialization:<br>
<pre>
&nbsp;&nbsp;&nbsp;&nbsp;Heir (value) : Object(value)<br>
&nbsp;&nbsp;&nbsp;&nbsp;{<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;...<br>
&nbsp;&nbsp;&nbsp;&nbsp;}
</pre>
where heir is a descendant of the Object class, and data types are omitted.<br>
Derived classes may also need copy constructors, move constructors, copy assignment operators, and move assignment operators. Since Object objects are not tied to the stored data type, within the Object class, data copying and moving should be unconditional - regardless of the type of data initially stored in the object. In this case, the Object class also needs to store the size of the allocated memory since copying an object containing data of another type may require reallocating memory. In this case, the Object class's constructor with a parameter will look as follows:<br><pre>
explicit Object(const T& value) {<br>
&nbsp;&nbsp;&nbsp;&nbsp;container = new T(value);<br>
&nbsp;&nbsp;&nbsp;&nbsp;destructor = [](void* ptr) -> void { delete (T*)ptr; };<br>
&nbsp;&nbsp;&nbsp;&nbsp;size = sizeof(T);<br>
}</pre>
Here, copyable is a concept that checks if the data type T of the constructor's argument allows copying the value using the copy constructor of this type, and the fields container, destructor, and size correspond to:<br>
1: a pointer to the memory area allocated for data storage;<br>
2: a function called for correct memory deallocation;<br>
3: the size of the allocated memory.<br>
The destructor field is set using a lambda function.<br>
To redefine the container's value, you should also define a corresponding method:<br><pre>
&nbsp;&nbsp;&nbsp;&nbsp;void setContainerValue(const T& value) {<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;if (destructor != nullptr) destructor(container);<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;container = new T(value);<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;size = sizeof(T);<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;destructor = [](void* ptr) -> void { delete (T*)ptr; };<br>
&nbsp;&nbsp;&nbsp;&nbsp;}</pre>
Thus, the Object class uses header files concepts, memory, and functional, which imposes a limitation on the version of the language used. Required version: C++20 and above.<br>
The task allows us to manage only wrappers for primitives. In this case, it is reasonable to create a template class and standardize all work with wrapped values. The wrapper should have minimal functionality provided by the necessity to override the pure virtual functions of the parent Object. It will also be useful to have the ability to implicitly convert types (class ⟷ primitive) and follow the "Rule of Five." Based on all the above, a conditional data storage scheme is composed in the program (Figure 2).<br>
&nbsp;&nbsp;&nbsp;&nbsp;<br><br>
<div align = center>
<img src=https://i.imgur.com/4s2GkQw.png alt="no img" />
</div>
<p align = center>Figure 2 - Data storage scheme</p>

<h3>Functional Description</h3>

<h4>Object and its Derivatives</h4>

<h4>Table 1 - Object structure, "rule of five" methods omitted.</h4>
<table border="1">
  <tr>
    <td colspan="6"><p>Object</p></td>
  </tr>
  <tr>
    <td rowspan="2"></td>
    <td colspan="2"><p>Modifiers and Keywords</p></td>
    <td rowspan="2"><p>Type, Name</p></td>
    <td rowspan="2"><p>Arguments</p></td>
    <td rowspan="2"><p>Comment</p></td>
  </tr>
  <tr>
    <td><p>Access</p></td>
    <td><p>...</p></td>
  </tr>
  <tr>
    <td rowspan="5"><p>Fields</p></td>
    <td rowspan="3"><p>private</p></td>
    <td></td>
    <td><p>void* container</p></td>
    <td rowspan="5"><p>—</p></td>
    <td><p>Pointer to the data stored by the object</p></td>
  </tr>
  <tr>
    <td></td>
    <td><p>size_t size</p></td>
    <td><p>Size of the stored data</p></td>
  </tr>
  <tr>
    <td></td>
    <td><p>std::function<void(void*)> destructor</p></td>
    <td><p>Function destructor for the stored data</p></td>
  </tr>
  <tr>
    <td rowspan="2"><p>protected</p></td>
    <td></td>
    <td><p>size_t typeIdentifier</p></td>
    <td><p>Identifier of the type used by derivatives as their own identifier</p></td>
  </tr>
  <tr>
    <td><p>const</p></td>
    <td><p>char* _typeName</p></td>
    <td><p>String with the type name, used by derivatives</p></td>
  </tr>
  <tr>
    <td rowspan="13"><p>Methods</p></td>
    <td><p>protected</p></td>
    <td></td>
    <td><p>void setContainerValue</p></td>
    <td><p>const T&*</p></td>
    <td><p>Properly copies the passed data into the object's storage</p></td>
  </tr>
  <tr>
    <td rowspan="12"><p>public</p></td>
    <td><p>explicit</p></td>
    <td><p>Object</p></td>
    <td><p>const T&</p></td>
    <td><p>Constructor</p></td>
  </tr>
  <tr>
    <td></td>
    <td><p>void* getValue()</p></td>
    <td><p>void</p></td>
    <td><p>Returns a reference to the memory area containing the object's data</p></td>
  </tr>
  <tr>
    <td></td>
    <td><p>size_t identifier()</p></td>
    <td><p>void</p></td>
    <td><p>Returns the value of the typeIdentifier field</p></td>
  </tr>
  <tr>
    <td><p>const</p></td>
    <td><p>char* typeName</p></td>
    <td><p>void</p></td>
    <td><p>Returns a pointer to the immutable _typeName</p></td>
  </tr>
  <tr>
    <td><p>virtual</p></td>
    <td><p>void initialize</p></td>
    <td><p>void</p></td>
    <td><p>Pure virtual function, freely overridden by derivatives for custom initialization (e.g., for initializing fields _typeName, typeIdentifier)</p></td>
  </tr>
  <tr>
    <td><p>virtual</p></td>
    <td><p>char* to_cstring</p></td>
    <td><p>void</p></td>
    <td><p>Pure virtual function, returns a dynamically allocated string containing the character representation of the stored value</p></td>
  </tr>
  <tr>
    <td><p>virtual</p></td>
    <td><p>void from_cstring</p></td>
    <td><p>const char*</p></td>
    <td><p>Pure virtual function, converts a string to a value and stores it in the object</p></td>
  </tr>
  <tr>
    <td><p>virtual</p></td>
    <td><p>void writeBinary</p></td>
    <td><p>std:ofstream&</p></td>
    <td><p>Pure virtual function, writes the value to a binary stream</p></td>
  </tr>
  <tr>
    <td><p>virtual</p></td>
    <td><p>void readBinary</p></td>
    <td><p>std:ifstream&</p></td>
    <td><p>Pure virtual function, reads the value from a binary stream and stores it in the object.</p></td>
  </tr>
  <tr>
    <td><p>virtual</p></td>
    <td><p>Object* getCopy</p></td>
    <td><p>void</p></td>
    <td><p>Pure virtual function, returns a dynamic copy of the object</p></td>
  </tr>
  <tr>
    <td><p>virtual</p></td>
    <td><p>std::partial_ordering compare</p></td>
    <td><p>const Object&</p></td>
    <td><p>Pure virtual function, compares objects with each other</p></td>
  </tr>
  <tr>
    <td><p>virtual</p></td>
    <td><p>void add</p></td>
    <td><p>const Object&</p></td>
    <td><p>Pure virtual function, adds objects</p></td>
  </tr>
</table>

<p>* Here and throughout Table 1, the template corresponds to the type whose object can be copied using a copy constructor.</p>

<p>The Object class takes on the memory management functionality. While different data types can be stored in the Object's storage, Object can correctly free them.</p>

<p>The code fragment shown below demonstrates the constructor of the Object class. It uses the copy constructor of the data type whose object is passed as an argument. The obtained copy is stored in dynamic memory. A function is created to act as the destructor for this type. This function will be used to release the memory pointed to by the container.</p>

<pre>
explicit Object(const T& value) {
    container = new T(value);
    destructor = [](void* ptr) -> void { delete (T*)ptr; };
    size = sizeof(T);
}
</pre>

<p>If necessary, the Object class is capable of providing limited dynamic typing when working with pointers (Int, Float - derived from Object):</p>

<pre>
Float floatNumber = 1.25;
Int intNumber = 2;
Object* floatNumber_ = &floatNumber;
Object* intNumber_ = &intNumber;
floatNumber_ = intNumber_;
std::cout << *((Int*)floatNumber_); // Output: 2
</pre>

<p>This happens and is considered correct behavior because data pointed to by the container field and the destructor functions are copied. The current type of the object can be determined using the typeIdentifier or _typeName fields, which are returned by the identifier() and typeName() methods, respectively. However, for correct horizontal type casting, derived classes should not have their own data, i.e., data not stored in Object.</p>

<p>Therefore, to design derived classes, the following conditions must be met:</p>

<ol>
  <li>Use the Object class functionality to store data. The interface is provided by constructors, assignment operators, and the setContainerValue() function.</li>
  <li>Implement all pure virtual functions.</li>
  <li>Provide initialization for the typeIdentifier and _typeName fields. It is recommended to override the initialize() function for this purpose.</li>
  <li>Do not have their own fields.</li>
</ol>

<p>In accordance with this, a template class called Primitive is created, and its structure is provided in Table 2.</p>

<h4>Table 2 - Structure of the Primitive class, "rule of five" methods and overridden functions omitted.</h4>

<table border="1">
  <tr>
    <td colspan="6"><p>Primitive</p></td>
  </tr>
  <tr>
    <td rowspan="2"></td>
    <td colspan="2"><p>Modifiers and Keywords</p></td>
    <td rowspan="2"><p>Type, Name</p></td>
    <td rowspan="2"><p>Arguments</p></td>
    <td rowspan="2"><p>Comment</p></td>
  </tr>
  <tr>
    <td><p>Access</p></td>
    <td><p>...</p></td>
  </tr>
  <tr>
    <td><p>Fields</p></td>
    <td><p>public</p></td>
    <td><p>static const</p></td>
    <td><p>size_t identifier</p></td>
    <td><p>—</p></td>
    <td><p>Stores the class identifier.</p></td>
  </tr>
  <tr>
    <td rowspan="4"><p>Methods</p></td>
    <td><p>private</p></td>
    <td></td>
    <td><p>T* convertContainer</p></td>
    <td><p>void*</p></td>
    <td><p>Helper method - converts the data received from Object::getValue() into a primitive type.</p></td>
  </tr>
  <tr>
    <td rowspan="3"><p>public</p></td>
    <td></td>
    <td><p>Primitive</p></td>
    <td><p>T</p></td>
    <td><p>Constructor that initializes the class with the received argument (the stored value).</p></td>
  </tr>
  <tr>
    <td></td>
    <td><p>T getValue()</p></td>
    <td><p>void</p></td>
    <td><p>Returns a copy of the current stored value.</p></td>
  </tr>
  <tr>
    <td></td>
    <td><p>operator T&amp;</p></td>
    <td><p>void</p></td>
    <td><p>Type conversion operator providing implicit type conversion to a primitive.</p></td>
  </tr>
</table>

<h3>Cyclical List</h3>

<p><code>clist</code> is a templated class. In the following text, the specialized template type is denoted as <code>T</code>. <code>clist</code> is contained in the file <code>clist.h</code>. Since the class is templated, all class functions are declared and defined in this file.</p>

<h4>1. Constructors and Destructors:</h4>

<pre>
clist() // Default constructor, creates an empty cyclical list.
clist(std::initializer_list&lt;T&gt;) // Constructor that takes a list of initializer elements and fills the current cyclical list with these elements. Example of usage: <code>clist&lt;int&gt; list = {1, 2, 3, 4, 5, 6}</code>.
clist(const clist&lt;T&gt;& other) // Copy constructor, creates the current object as a copy of the <code>clist</code> <em>other</em>.
clist(clist&lt;T&gt;&& other) // Move constructor, creates the current object by moving the contents of the <code>clist</code> <em>other</em> into the current one.
~clist() // Destructor, frees resources allocated for storing elements of the cyclical list.
</pre>

<h4>2. Assignment Operators:</h4>

<pre>
clist&amp; operator=(const clist&lt;T&gt;& other) // Copy assignment operator, copies the contents of <code>clist</code> <em>other</em> into the current cyclical list.
clist&amp; operator=(clist&lt;T&gt;&& other) // Move assignment operator, moves the contents of <code>clist</code> <em>other</em> into the current cyclical list.
</pre>

<h4>3. List Modification Methods:</h4>

<pre>
void push_back(T) // Adds an element to the end of the list.
void push_front(T) // Adds an element to the beginning of the list.
void insert(iterator, T) // Inserts an element before the element pointed to by the iterator.
void erase(iterator begin, iterator end) // Deletes elements in the range [<em>begin</em>, <em>end</em>) from the list.
void erase(iterator) // Deletes the element pointed to by the iterator.
void clear() // Clears the entire cyclical list.
void pop_back() // Deletes the last element from the list.
void pop_front() // Deletes the first element from the list.
void shift_forward() // Moves the list elements to the left (circular shift).
void shift_backward() // Moves the list elements to the right (circular shift).
</pre>

<h4>4. List Iteration Methods:</h4>

<pre>
auto foreach(std::function&lt;void(T&)&gt;) // Executes the specified function for each element in the list. The function should take an element or a reference to an element.
auto foreach(std.function&lt;void(const T&)&gt;) const // Executes the specified function for each element in the list. The function should take an element or a constant reference to an element.
</pre>

<h4>5. List Content Information Methods:</h4>

<pre>
bool empty() const: Checks if the list is empty.
size_t size() const: Returns the current size of the list.
</pre>

<h4>6. List Element Access Methods:</h4>

<pre>
T& front() // Returns a reference to the first element in the list.
T& back() // Returns a reference to the last element in the list.
const T& front() const // Returns a constant reference to the first element in the list.
const T& back() const // Returns a constant reference to the last element in the list.
</pre>

<h4>7. Iterators:</h4>

<p><code>clist::iterator</code>: Iterator class for navigating and accessing list elements. The iterator allows both reading and modifying elements in the list and supports dereferencing (*), incrementing (++ and --), comparing (==), and assignment (=) operations.</p>

<p><code>clist::const_iterator</code>: Class for constant iteration and accessing list elements without modification. It supports dereferencing (*), returning a reference to a constant object, incrementing (++ and --), comparing (==), and assignment (=) operations.</p>

<p>Initialization of iterator data is only possible using the assignment operator in relation to the value returned by the <code>begin()</code> function of the container that the iterators should be associated with. The iterators themselves store the associatedContainer field, which contains a reference to the list they are associated with.</p>
<pre>
<em>class iterator</em> {
    private:
        Node* ptr = nullptr;
        const clist&lt;T&gt;* associatedContainer = nullptr;
        friend class clist&lt;T&gt;;
        ...
}
</pre>

<p>In turn, the list methods that use iterators perform a check to determine if the passed iterator is associated with the current container. This ensures that the container instance is protected from the use of uninitialized iterators or iterators associated with other lists. In cases where an iterator is not associated with any list (uninitialized) or associated with a different list, the method will throw an exception of type <code>std::logic_error</code> with an error description. An example from the <code>erase(iterator begin, iterator end)</code> function:</p>

<pre>
...
if (begin.associatedContainer != this || end.associatedContainer != this)
    throw std::logic_error("erase: using an iterator that isn't linked to this container");
...
</pre>

<p>An iterator obtained from an empty container will point to a nonexistent element. Any operations on such an iterator, except for comparison (==) and assignment (=), are not allowed and will result in an exception. Such an iterator becomes invalid when the container contains at least one element. The container is protected from the use of such iterators, and when used with a non-empty list, it will throw an exception. The behavior of an empty container when using such iterators is defined as follows:</p>

<ol>
    <li>insert(iterator) // Inserting an element at the beginning.</li>
    <li>erase(iterator) // No action is taken.</li>
</ol>

<p>In this case, the <code>erase(iterator, iterator)</code> method will throw an exception.</p>

<p>An iterator pointing to a deleted element becomes invalid. The class is not protected from their use since validating such iterators is a resource-intensive operation. Exceptions thrown when using iterators of this kind are impossible to catch and handle.</p>


