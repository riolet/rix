Rix Language
================

Rix is an expressive and efficient language for the C ecosystem. Rix uses C libraries natively and generates easy to understand C code.

There are five core principles behind Rix:

1. Ease of programming, inspired by Python
2. Fast, like C code
3. Type safety, like Scala
4. Ability to go lower level and write C code
5. No reserved keywords, everything is redefinable

We ran the [prime counting benchmark](https://bjpelc.wordpress.com/2015/01/10/yet-another-language-speed-test-counting-primes-c-c-java-javascript-php-python-and-ruby-2/) for Rix, C and Python. Here's what we got:

| Measure            | Rix     | C   | Python |
|--------------------|---------|-----|--------|
| Runtime (ms)       | 457     | 424 | 7836   |
| Characters of code | 413     | 542 | 474    |

The best way to introduce yourself to this language is to take a look at some of the examples and try running them.  Rix is still in early development; many features have not yet been fully implemented or described.  A list of major [development tasks remaining](https://github.com/riolet/rix/wiki/State-of-the-Compiler) can be found in the wiki.

Features
========

* Rix uses type inferencing, so the boilerplate Java statement:

```
Point point = new Point (x,y)
```

  becomes a short Rix statement:

```
#point = Point (x, y)
```

New identifiers have are marked as new with "#", and Rix will infer their type for you. Their types cannot be changed once set. In the above example, `point` gets category `Point`.

* Rix provides exception-safe resource management using Resource Acquisition Is Initialization (RAII), so you don't have to worry about memory leaks and unclosed file handles.


* Rix tries to follow English linguistic constructs wherever possible and practical, so, most of Rix language expressions take the form:

  Subject Verb Object

  The following statements:

```
hello = "Hello"
helloWorld.add(hello)
```
parse as:


| Subject | Verb | Object  |
|---------|------|---------|
| hello   | =    | "Hello" |
| myset   | add  | hello   |


Subjects and objects are collectively known as nouns, and are analogous to object (instances of a class) in object-oriented programming.

Methods and function are called *verbs* in ritchie. For example:

```
factorial -> int (int n)
	#result = 1
	#i.for (1,n+1)
		result = result * i
	-> result

print (factorial (5))
```

You can also write succint one liner verbs like the one below which gives the nth Fibonacci number:

```
fib -> int(int n) = (n <= 1).tf (n, fib(n-1) + fib(n-2))

print (fib(5))
```

There's no assignment operator in Rix, but `=` is defined as an assignment verb for `Identifier`.

* A special category of verb is a control flow verb.

`if`, `while` and `for`  in Rix are all such verbs. They are not keywords, as you can redefine them, although this is probably not a good idea.
* Rix has no keywords.  All operators in Rix can be overloaded.


* Rix is whitespace sensitive


Get started
===========
1. Build the rix compiler `make clean;make`
2. Write your rix program in your favourite text editor (let's call it program.rit)
3. Set RIX_HOME `export  RIX_HOME=/path/to/rix`
4. Run `${RIX_HOME}/rix.sh program.rit` and rix will build, execute and run the program

Team
====
Rix Language is being developed by a group of efficiency obsessed programmers for demanding programmers who want both the conciseness of Python and the efficiency of C.

Concept: [Rohana Rezel](https://github.com/rrezel) ([Riolet Corporation](https://github.com/riolet))

Design and implementation: [Joe Pelz](https://github.com/JoePelz), [Phillip Hood](https://github.com/pvgh) and [Dimitry Rakhlei](https://github.com/DimitryRakhlei) (final year students at BCIT, Burnaby, BC, Canada)

