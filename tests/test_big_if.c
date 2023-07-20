int main() {
	if(1) {
		printf("vai imprimir\n");
	}
	if(0) {
		printf("não vai imprimir\n");
	}

	int i = 0;
	float j = 6.56;
	if(i && j) {
		printf("i && j\n");
	}
	if(i || j) {
		printf("i || j\n");
	}
	if(1 && j) {
		printf("1 && j\n");
	}
	if(1 && 1 || i) {
		printf("1 && 1 || i\n");
	}
	if(1 && 0 || j) {
		printf("1 && 0 || j\n");
	}
	if (1 && 0 || i) {
		printf("1 && 0 || i\n");
	}
	if (j && j > 5) {
		printf("j && j > 5\n");
	}
	return 0;
}
