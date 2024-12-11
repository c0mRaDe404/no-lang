n = 5
i = 1

while i <= n:
    factorial = 1
    j = 1

    while j <= i:
        factorial *= j
        j += 1

    sum_of_digits = 0
    temp = factorial

    while temp > 0:
        digit = temp % 10
        sum_of_digits += digit
        temp //= 10

    is_prime = True
    k = 2

    while k * k <= sum_of_digits:
        if sum_of_digits % k == 0:
            is_prime = False
            break
        k += 1

    if is_prime and sum_of_digits > 1:
        print(i)
        print(sum_of_digits)
        print(1)
    else:
        print(i)
        print(sum_of_digits)
        print(0)

    i += 1

