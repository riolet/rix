Ritchie Language
================

Ritchie is an expressive and efficient language for the C ecosystem. Ritchie uses C libraries natively and generates easy to understand C code. It is named after the great computer scientist Dennis Ritchie, if not for whom we would all be programming in Visual Basic.

There are five core principles behind Ritchie:

1. Ease of programming, inspired by Python
2. Fast, like C code
3. Type safety, like Scala
4. Ability to go lower level and write C code
5. No reserved keywords, everything is redefinable

We ran the [prime counting benchmark](https://bjpelc.wordpress.com/2015/01/10/yet-another-language-speed-test-counting-primes-c-c-java-javascript-php-python-and-ruby-2/) for Ritchie, C and Python. Here's what we got:

|                    | Ritchie | C   | Python |
|--------------------|---------|-----|--------|
| Runtime (ms)       | 457     | 424 | 7836   |
| Characters of code | 423     | 542 | 474    |

The best way to introduce yourself to this language is to take a look at some of the examples and try running them.  Ritchie is still in early development; many features have not yet been fully implemented or described.  A list of major [development tasks remaining](https://github.com/riolet/ritchie/wiki/State-of-the-Compiler) can be found in the wiki.

Features
========

* Ritchie uses category inferencing, so the boilerplate Java statement:

```
Point point = new Point (x,y)
```

  becomes a short Ritchie statement:

```
#point = Point (x, y)
```
Identifiers have their category inferred if they are marked as new with "#". Their types cannot be changed once set. In the above example, `point` gets category `Point`.


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


* Ritchie has no keywords.  All operators in Ritchie can be overloaded.


* We call verbs the constructs called functions, methods or subroutines in other languages. For example:

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

There's no assignment operator in Ritchie, but `=` is defined as an assignment verb for `Identifier`.

* A special category of verb is a control flow verb.

`if`, `while` and `for`  in Ritchie are all such verbs. They are not keywords, as you can redefine them, although this is probably not a good idea.

* Ritchie is whitespace sensitive



Get started
===========
1. Build the ritchie compiler `make clean;make`
2. Write your ritchie program in your favourite text editor (let's call it program.rit)
3. Set RITCHIE_HOME `export  RITCHIE_HOME=/path/to/ritchie`
4. Run `${RITCHIE_HOME}/ritchie.sh program.rit` and ritchie will build, execute and run the program

Team
====
Ritchie Language is being developed by a group of efficiency obsessed programmers for demanding programmers who want both the conciseness of Python and the efficiency of C.

Concept: [Rohana Rezel](https://github.com/rrezel) ([Riolet Corporation](https://github.com/riolet))

Design and implementation: [Joe Pelz](https://github.com/JoePelz), [Phillip Hood](https://github.com/pvgh) and [Dimitry Rakhlei](https://github.com/DimitryRakhlei) (final year students at BCIT, Burnaby, BC, Canada)

