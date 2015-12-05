Ritchie Language
================

Ritchie is an expressive and efficient language for the C eco system. Ritchie uses C libraries natively, and generates easy to understand C code. It is named after the great computer scientist Dennis Ritchie, if not for whom we would all be programming in Visual Basic.

There are four core principals behind Ritchie:

1. Ease of programming, inspired by Python
2. Fast, like C code
3. Ability to go lower level and write C code
4. No reserved keywords, everything is redefinable

We ran the [prime counting benchmark](https://bjpelc.wordpress.com/2015/01/10/yet-another-language-speed-test-counting-primes-c-c-java-javascript-php-python-and-ruby-2/) for Ritchie, C and Python, and here's what we go:

|                    | Ritchie | C   | Python |
|--------------------|---------|-----|--------|
| Runtime (ms)       | 457     | 424 | 7836   |
| Characters of code | 423     | 542 | 474    |

The best way to introduce yourself to this language is to take a look at some of the examples, and try running them yourself.  Ritchie is still in early development and many features have not yet been fully implemented and described.  A list of major [development tasks remaining](https://github.com/riolet/ritchie/wiki/State-of-the-Compiler) can be found in the wiki.

Features
========

* Ritchie uses type inferencing, so the boilerplate Java statement:

```
Point point = new Point (x,y)
```

  becomes a short Ritchie statement:

```
point = Point x, y
```
Identifiers are have their type inferred when they are first used and their type cannot be changed after that. In the above example, point gets type Point.


* Ritchie tries to follow English linguistic constructs, so, most of Ritchie language expressions take the form:

  Subject Verb Object

  The following phrase:

```
hello = "Hello"
```
parses as:


| Subject | Verb | Object  |
|---------|------|---------|
| hello   | =    | "Hello" |


* Ritchie has no keywords.  There are many symbols with predefined meanings, but the intention of Ritchie is to have all those symbols redefineable.  Currently Ritchie has been developed with a standard dialect of ritchie in mind, and to that effect many symbols work as keywords might otherwise have in other languages.  For example to define a class in Ritchie, you would type
```
SomeClass :: SomeBaseClass
```
  The `::` is simply a verb that reads "become a subclass of".


* We call verbs what the constructs called functions, methods or subroutines in other languages. For example:

```
Integer : factorial Integer n
	result = 1
	i for 1,n+1
		result = result * i
	-> result

print factorial 5
```

There's no assignment operator in Ritchie, but `=` is defined as an assignment verb for `Identifier`.

* A special type of verb is a control flow verb.

`if`, `while` and `for`  in Ritchie are all such verbs. They are not keywords, as you can redefine them, although this is probably not a good idea.

* Ritchie is whitespace sensitive


Team
====
Ritchie Language is being developed by a group of efficiency obsessed programmers for demanding programmers who want both the conciseness of Python and the efficiency of C.

Concept: [Rohana Rezel](https://github.com/rrezel) ([Riolet Corporation](https://github.com/riolet))

Design and implementation: [Joe Pelz](https://github.com/JoePelz), [Phillip Hood](https://github.com/pvgh) and [Dimitry Rakhlei](https://github.com/DimitryRakhlei) (final year students at BCIT, Burnaby, BC, Canada)

