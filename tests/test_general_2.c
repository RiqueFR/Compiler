/* arquivo de teste */
int max(int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
	return b;
}

int test() {
    int x;
    x = 2 + 3 * 4;
    return x;
}

int cond() {
    int x = 1;
    int y = 2;
    if (x > y) {
        return x;
    } else {
        return y;
    }
	return y;
}

int main() {
    int x = 0;
    int y = 5;

    while (x < y) {
        x = x + 1;
		printf(x);
    }
	y = y + 5;

    int z = max(x, y);
	printf(z);
    return z;
}
