assure(int truth, char* message) {
  if (!truth) {
    printf("%s", message);
    exit(0);
  }
}
