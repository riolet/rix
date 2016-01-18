Rix Tutorial
===================


In this Tutorial, we'll learn how to write a useful program using Rix.

----------

What does a simple program in Rix language look like?
----------------------------------------------------------------------
The Fibonacci Sequence is the series of numbers: 1, 1, 2, 3, 5, 8, 13, 21, 34, ... where the next number is found by adding the two previous numbers. By convention, the first two numbers are set to 1.

Imagine we were tasked with writing a program that displays all the Fibonacci numbers from 1 to N, where N is supplied by a user. This is what our first attempt would look like:

    fib -> int(int n) = (n <= 1).tf (n, fib (n-1) + fib (n-2))

    if (args()>1)
        #N = int (args (1))

        #i.for (1, N+1)
            print (i + ": " + fib (i))

    else
        print ("Usage "+args(0)+" N")

`fib -> int(int n)` declares the verb (a function in this case) `fib` that returns an `int` and taking an `int` as a parameter.

If `n <= 1` then `fib` returns `n`, otherwise, `fib` recursively returns `fib(n-1) + fib(n-2)`.  Notice here that Rix, like Python is white space is sensitive, so code blocks are marked by indentation.

Then, in the main body of the program, we check if there are more than one argument (the first argument, argument 0, is always the path to the executable), get the first command line argument using `args 1`, create a new integer object using that, and assign it to a new variable `N`. The `#` before an identifier indicates that we're declaring a new variable.

Rix is a statically typed language, so `N` needs a category. But we don't need to specify `N`'s category in advance, as Rix compiler uses category inferencing to figure out what category `N` needs to be, which happens to be `int` in this case.

`#i for 1, N+1` is applied to the code block `print (i + ": " + (fib i))` immediately following the `for` verb. We have declared a new variable called `i` that `for` uses as an index, and `i` will go from `i=1` to `i<N+1`.

The C language equivalent of this would be:

    int i;
    for ( i=1 ; i<N+1 ; i++) {
	    *codeblock*
    }

The Python language equivalent of this would be:

    for i in range(1, N+1):
    	*codeblock*

`print` prints the parameter to standard output, and `i + ": " + (fib i)` concatenates `int i`, `String ": "` and `int (fib i)`.

If the user hasn't supplied an argument, it just prints the usage. As Rix has no keywords, `if`, `else` and `for` are also verbs.

How can we make a more succint version of our Fibonacci program?
----------------------------------------------------------------

We can replace the standard `fib` function above with a single expression function (SEF):

    fib -> int (int n) = (n <= 1).tf (n, fib (n-1) + fib (n-2))

Here, `fib -> int:  int n = ` is the function header and `(n <= 1) tf n, (fib (n-1)) + (fib (n-2))` is the expression. The expression is returned in a SEF.

The `int` category's `<=` verb returns a `bool` category object, which is true if the subject, which in this case is `n` is less than or equal to `1`, which is the object. Please see the section *What's the whole Subject Verb Objects (SOV) business?* for more details.

`bool` category's `tf` verb, where `tf` stands for **t**rue or **f**alse, returns the first parameter if the `bool` subject is `true`, and the second if the `bool` subject is false. The verb `tf` takes two generic parameters, and returns the category of the first parameter.


How do we do *99 bottles of beer* in Rix?
---------------------------------------------
Here's the (in)famous 99 bottles of beer program in Rix.

	line -> String(int b, String end)
	    if (b>1)
	        -> "" + b + " bottles " + end
	    if (b==1)
	        -> "" + b + " bottle " + end
	    -> "No more bottles " + end

	#i = 99

	while (i>0)
	    print (line (i, "of beer on the wall,"))
	    print (line (i, "of beer,"))
	    print ("Take one down, pass it around,")
	    print (line (i-1, "of beer on the wall.\n"))
	    i -= 1


We have verb call `line` that returns a `String` taking `int b` (number of bottles) and `String end` as parameters.

If we have more than one bottle, the function returns the plural form `"" + b + " bottles " + end`
If we have exactly one bottle, the function returns the singular form `"" + b + " bottle " + end`
If we have no bottles, well we're not drinking then.

In the main body of the function, we declare a new variable `i` (the number of bottles) marking it with `#` to indicate that it's a new variable, and then assign the integer value 99 to it.

Then, while `i` is greater than 0,  we call  our function `line` with the number of bottles `i` and the ending we want for the line.

In the last line, we decrement `i` by 1.

How can we improve our *99 bottles of beer* example?
----------------------------------------------------------------

We can simplify the `line` verb by using Ternary logic functions.

	bottles -> String (int b) = (b<>1).gel(b+" bottles", "1 bottle", "No more bottles")

The Ternary comparison (b<>1) returns either less than `lt`, equals `eq` or greater than `gt`. The verb `gel` stands for `g`reater than `e`quals `l`ess than.

Now our function has been reduced to a single return expression. We can make it even tighter using a single expression function.


How do we do object oriented programming with Rix?
---------------------------------------------------------------------

Here's a simple objected oriented program in Rix:

	Rectangle :: BaseType
	  w = int
	  h = int

	  $(int w, int h)
	    $.w = w
	    $.h = h

	  area->int() = w * h

	  perimeter->int() = ( w + h ) *2


	NamedRectangle :: Rectangle

	  name = String

	  $(int w, int h, String name)
	    $.w = w
	    $.h = h
	    $.name = name

	  nameArea->String() = "NamedRectangle "+ name + " has an area of "+$.area()


	#r = Rectangle (5, 10)
	#s = NamedRectangle (3, 4, "Great")

	print (s.name)
	print (r.area() + " " + r.perimeter())
	print (s.name + " " + s.area())
	print (s.nameArea())

The `::` symbol read as *subtype of*, thus `Rectangle` becomes a subtype of `BaseType`.

Next we declare the fields with `width` and  `height`, where both happen to be integers.

Then we start working on `Rectangle`'s verbs, starting with the constructor `$(int w, int h)` which takes `width` and  `height` as parameters and sets the fields `width` and  `height` to those parameters respectively.

We also add a verb called `area`, which simply returns an integer equal to `width * height`, and perimeter, which returns `(width * height)*2`.

There we have it, our `Rectangle` is ready, but what if we want to add a `NamedRectangle` now?

We can simply make `NamedRectangle` a subtype of `Rectangle`.  We only need to change the constructor to take just one parameter additional parameter: `name`.

What's the whole Subject Verb Objects (SOV) business?
------------------------------------------------------------------

Every expression in Rix tries to follow the English language word order in the linguistic form:
`Subject Verb Objects`

`Subject` is optional, and `Objects` can be zero or more, and are comma separated.

So, `2.35 + 1` is evaluated as

| Subject | Verb | Object  |
|---------|------|---------|
| 2.35  | +   | 1 |

The subject is a  `float`, and the object is an `int`. Rix calls `float` category's `+` verb which takes an `int`  as a parameter.
