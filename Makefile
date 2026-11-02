CC     = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -g
OBJS   = main.o util.o colecao.o cliente.o veiculo.o venda.o

erp: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c $(wildcard *.h)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) erp

.PHONY: clean
