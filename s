let n = 5;
let i = 1;

while (i <= n) {
    let factorial = 1;
    let j = 1;

    while (j <= i) {
        factorial = factorial * j;
        j = j + 1;
    }

    let sum_of_digits = 0;
    let temp = factorial;

    while (temp > 0) {
        let digit = temp % 10;
        sum_of_digits = sum_of_digits + digit;
        temp = temp / 10;
    }

    let is_prime = true;
    let k = 2;

    while (k * k <= sum_of_digits) {
        if (sum_of_digits % k == 0) {
            is_prime = false;
            break;
        }
        k = k + 1;
    }

    if (is_prime and sum_of_digits > 1) {
        print(i);
        print(sum_of_digits);
        print(1);
    } else {
        print(i);
        print(sum_of_digits);
        print(0);
    }

    i = i + 1;
}

