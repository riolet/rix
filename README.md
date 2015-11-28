Ritchie Language
================

Ritchie is an expressive and efficient language for the C eco system. Ritchie uses C libraries natively, and generates easy to understand C code. It is named after the great computer scientist Dennis Ritchie, if not for whom we would all be programming in Visual Basic.

There are four core principals behind Ritchie:  

1. Ease of use, inspired by Python
2. Fast, like C code
3. Ability to go lower level and write C code
4. No reserved keywords, everything is redefinable


The best way to introduce yourself to this language is to take a look at some of the examples, and try running them yourself.  Ritchie is still in early development and many features have not yet been fully implemented and described.  A list of major [development tasks remaining](https://github.com/riolet/ritchie/wiki/State-of-the-Compiler) can be found in the wiki.

Features
========

Ritchie Language is being developed by a cloud DevOps engineer and a team of BCIT students for lazy ~~programmers~~ cloud DevOps engineers who want both the conciseness of Python and the efficiency of C.

* Ritchie uses type inferencing, so the boilerplate Java statement:

```
SomeClass someInstance = new SomeClass (someArgument)
```

  becomes a short Ritchie statement:  
  
```
someInstance = SomeClass someArgument
```
  Smooth, eh?


* Ritchie tries to follow English linguistic constructs, so, most of Ritchie language expressions take the form:

  Subject Verb Object  

  Currently, the grammar of Ritchie Language is as follows:  


```
Program := Expressions
Expressions := Expression | Expression, NEWLINE, {Expressions}
Expression := Subject Verb Objects
Objects := Object | Object, ',', {Objects}
Subject := Noun | '(' Expression ')'
Object := Noun | Expression | '(' Expression ')'
Noun := Integer | Float | String | Identifier
Verb := '+' | '>' | '=' | "print" | ...
```

  The following phrase:

```
hello = "Hello"
```
parses as:  

subject: hello

verb: =

object: "Hello"


* Identifiers are have their type inferred when they are first used and their type cannot be changed after that. In the above example, hello gets type String. 


* Ritchie has no keywords.  There are many symbols with predefined meanings, but the intention of Ritchie is to have all those symbols redefineable.  Currently Ritchie has been developed with a standard dialect of ritchie in mind, and to that effect many symbols work as keywords might otherwise have in other languages.  For example to define a class in Ritchie, you would type
```
SomeClass : SomeBaseClass
```
The `:` represents class definition right now, but it needn't mean that in the future. 


* We call verbs what the constructs called functions, methods or subroutines in other languages

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
Integer := factorial Integer n
	result = 1
	i for 1,n+1
		result = result * i.
	-> result
```	

  we could then call the function as  
  
```
  result factorial 5
```

  which would first create the variable result and then assign 5! to it.
  
  There's no assignment operator in Ritchie, but `=` is defined as an assignment verb for `BaseType`.  
  
* A special type of verb is a control flow verb. 

`if`, `while` and `for`  in Ritchie are all such verbs. They are not keywords, as you can redefine them, although this is probably not a good idea.

