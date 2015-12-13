Ritchie Tutorial
===================


In this Tutorial, we'll learn how to write a useful program using Ritchie.

----------


What does a simple Ritchie Program look like?
-------------------------------------------------------
Here's the (in)famous 99 bottles of beer program in Ritchie.

    line -> String: Integer b, String end
        if (b>1)
            -> "" + b + " bottles " + end
        if (b==1)
            -> "" + b + " bottle " + end
        -> "No more bottles " + end

    #i = 99
    while (i>0)
        print (line i, "of beer on the wall,")
        print (line i, "of beer,")
        print "Take one down, pass it around,"
        print (line (i-1), "of beer on the wall.\n")
        i-=1


We have verb call `line` that returns a `String` taking `Integer b` (number of bottles) and `String end` as parameters.

If we have more than one bottle, the function returns the plural form `"" + b + " bottles " + end`
If we have exactly one bottle, the function returns the singular form `"" + b + " bottle " + end`
If we have no bottles, well we're not drinking then.

In the main body of the function, we declare a new variable `i` (the number of bottles) marking it with `#` to indicate that it's a new variable, and then assign the integer value 99 to it.

Then, while `i` is greater than 0,  we call  our function `line` with the number of bottles `i` and the ending we want for the line.

In the last line, we decrement `i` by 1.

How can we improve our simple example?
--------------------------------------------------

We can simplify the `line` verb by using Ternary logic functions.

    line -> String: Integer b, String end
        -> (b<>1) gel ("" + b + " bottles " + end),
			       ("" + b + " bottle " + end),
			       ("No more bottles " + end)


The Ternary comparison (b<>1) returns either less than `lt`, equals `eq` or greater than `gt`. The verb `gel` stands for `g`reater than `e`quals `l`ess than.


Now our function has been reduced to a single return expression. We can make it even tighter using a single expression function

    line -> String: Integer b, String end = (b<>1) gel ("" + b + " bottles " + end),
												       ("" + b + " bottle " + end),
												       ("No more bottles " + end)


How do we do object oriented programming with Ritchie?
---------------------------------------------------------------------

Here's a simple objected oriented program in Ritchie:

    Rectangle :: BaseType

      width = Integer
      height = Integer

      ::: Integer width, Integer height
        $.width = width
        $.height = height

      area -> Integer: = width * height


    Square :: Rectangle

      ::: Integer edge
        width = edge
        height = edge


    #r = Rectangle 5, 10
    #s = Square 16

    print ("Rectange area " + (r area))
    print ("Square Area " + (s area))

The `::` symbol read as *subtype of*, thus `Rectangle` becomes a subtype of `BaseType`.

Next we declare the fields with `width` and  `height`, where both happen to be integers.

Then we start working on `Rectangle`'s verbs, starting with the constructor `:::` which takes `width` and  `height` as parameters and sets the fields `width` and  `height` to those parameters respectively.

We also add a verb called `area`, which simply returns an integer equal to `width * height`.

There we have it, our `Rectangle` is ready, but what if we want to add a `Square` now?

We can simply make `Square` a subtype of `Rectangle`.  We only need to change the constructor to take just one parameter `edge` and set both `width` and `height` to be `edge`.
