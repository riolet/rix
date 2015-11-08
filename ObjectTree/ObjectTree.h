typedef enum {
  Undefined,
  Variable,
  Type,
  Constructor,
  Function,
  CodeBlock,
  Expression,
} TYPE;

typedef struct _Object      Object;
typedef struct _ListObject  ListObject;
typedef struct _ListString  ListString;
typedef struct _ListType    ListType;

struct _ListString {
  const char*  value;
  ListString*  next;
};
struct _ListObject{
  Object*      value;
  ListObject*  next;
};

struct _Object{
  const char*        name;           //symbol name     ("myInteger", "calcTotalArea ", "Rectangle")
  const char*        fullname;       //symbol fullname ("myInteger", "Integer_calcTotalArea_Rectangle_Rectangle", "BaseType_Rectangle")
  Object*      parentScope;    //parent scope    (global scope, global scope, BaseType)
  TYPE         type;           //What is this?   (Variable, Function, Class)
  const char*        returnType;     //What value type?(Integer,  Integer,  NULL)
  ListString*  paramTypes;     //parameters?     (NULL,     [Integer, Integer], NULL)
  ListObject*  definedSymbols; //Things inside?  (NULL, [Rectangle "r1", Rectangle "r2", Integer "a1", Integer "a2"], [Integer "w", Integer "h", Constructor "Rectangle", Function "Area"])
  ListString*  code;           //CodeBlock       (NULL, "Integer ...calcTotalArea...(...) {...", "typedef struct...")
};

//mallocs memory and returns a pointer to a new Object
Object * CreateObject(const char* name, const char* fullname, Object* parent, TYPE type, const char* returnType);

//append item to end of linked list
void addParam(Object* tree, const char* type);
void addSymbol(Object* tree, Object* leaf);
void addCode(Object* tree, const char* line);

//writes the code of root first, then children in order
//void writeTree(FILE* outc, FILE* outh, Object* tree);

//searches for identifier in current, and parent scope.
//returns Undefined if identifier isn't found.
TYPE getIdentType(Object* scope, char* identifier);
Object* findByName(Object* scope, char* name);


