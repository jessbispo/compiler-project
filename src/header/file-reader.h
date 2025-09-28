/* @brief Lê o conteúdo de um arquivo e armazena em um buffer.
 * @param filename Nome do arquivo a ser lido.
 * @param buffer Ponteiro para o buffer onde o conteúdo do arquivo será armazenado.
 * @param size Ponteiro para o tamanho do buffer.
 * @return 0 em caso de sucesso, -1 em caso de erro.
 */
int read_file(char *filename, char **buffer, size_t *size);