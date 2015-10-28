typedef enum {
  Undefined,
  Variable,
  Type,
  Constructor,
  Function,
  CodeBlock
} TYPE;

typedef struct _Object      Object;
typedef struct _ListObject  ListObject;
typedef struct _ListString  ListString;
typedef struct _ListType    ListType;

struct _ListString {
  char*        value;
  ListString*  next;
};
struct _ListType{
  TYPE         value;
  ListType*    next;
};
struct _ListObject{
  Object*      value;
  ListObject*  next;
};

struct _Object{
  char*        name;           //symbol name     ("myInteger", "calcTotalArea ", "Rectangle")
  char*        fullName;       //symbol fullname ("myInteger", "Integer_calcTotalArea_Rectangle_Rectangle", "BaseType_Rectangle")
  Object*      parentScope;    //parent scope    (global scope, global scope, BaseType)
  TYPE         type;           //What is this?   (Variable, Function, Class)
  char*        returnType;     //What value type?(Integer,  Integer,  NULL)
  ListType*    paramTypes;     //parameters?     (NULL,     [Integer, Integer], NULL)
  ListObject*  definedSymbols; //Things inside?  (NULL, [Rectangle "r1", Rectangle "r2", Integer "a1", Integer "a2"], [Integer "w", Integer "h", Constructor "Rectangle", Function "Area"])
  ListString*  code;           //CodeBlock       (NULL, "Integer ...calcTotalArea...(...) {...", "typedef struct...")
};

//mallocs memory and returns a pointer to a new Object
Object * CreateObject();
void setName(char* name);
void setFullName(Object* tree, char* fullname);
void setParentScope(Object* tree, Object* parent);
void setReturn(Object* tree, char* return);

//append item to end of linked list
void addParam(Object* tree, TYPE type);
void addSymbol(Object* tree, Object* leaf);
void addCodeLine(Object* tree, char* line);

//writes the code of root first, then children in order
void writeTree(FILE* output, Object* tree); 

//searches for identifier in current, and parent scope. 
//returns Undefined if identifier isn't found.
TYPE getIdentType(Object* scope, char* identifier);


