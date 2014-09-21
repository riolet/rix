Ritchie Language
================

Ritchie is an expressive and efficient language for the C eco system. Ritchie uses C libraries natively, and generates easy to understand C code. It is named after the great computer scientist Dennis Ritchie, if not for whom we would all be programming in Visual Basic.

Features
========

Ritchie Language is being developed by a lazy ~~programmers~~ cloud DevOps engineer for lazy ~~programmers~~ cloud DevOps engineers who want both the conciseness of Python and the efficiency of C.

1. Ritchie uses type inferencing, so the boilerplate Java statement:

```
SomeClass someInstance = new SomeClass (someArgument)
```

  becomes a short Ritchie statement:  
  
```
someInstance = SomeClass someArgument
```
  Smooth, eh?


2. Ritchie tries to follow English linguistic constructs, so, most of Ritchie language expressions take the form:

  Subject Verb Object  

  Currently, the grammar of Ritchie Language is as follows:  


```
Program := Expressions
Expressions := Expression | Expression, NEWLINE, {Expressions}
Expression := Subject Verb Objects
Objects := Object | Object, ',', {Objects}
Subject := Noun | '(' Expression ')'
Object := Noun | Expression | '(' Expression ')'
```

  The following parses as:  

```
hello = "Hello"
```

subject: hello

verb: =

object: "Hello"



3. Every unknown identifier belongs to the built in class Identifier. Nouns of the Identifier type are the only nouns that can change type.

  In  

```
hello = "Hello"
```

  hello starts out life as an "Identifier" but becomes a "String".  


4. We are still debating whether to support operator precedence or not. 

  Right now  


```
f = 2 * 3 + 4
```


  is parsed right to left as  


```
f = (2 * (3+4))
```

5. Ritchie has no keywords. More on that later.


6. We call verbs what the constructs called functions, methods or subroutines in other languages

  Plain vanilla verbs behave like C functions  
  
```
Integer: factorial Integer n
	result = 1
	i for 1,n+1
		result = result * i.
	->result.
```

  Had we written the the above verb as an assignment verb
	
```
Integer: factorial Identifier ident, Integer n
	result = 1
	i for 1,n+1
		result = result * i.
	ident=result.
```	

  we could then call the function as  
  
```
  result factorial 5
```

  which would first create the variable result and then assign 5! to it.
  
  There's no assignment operator in Ritchie, but `=` is defined as an assignment verb for `BaseType`.  
  
7. A special type of verb is a control flow verb. 

`if`, `while` and `for`  in Ritchie are all such verbs. They are not keywords, as you can redefine them, although this is probably not a good idea.

