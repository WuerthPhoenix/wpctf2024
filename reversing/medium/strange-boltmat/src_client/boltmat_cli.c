#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define IN_BUF_SIZE 0x100
#define MAX_ITEMS_NAME_LEN 0x20

struct catalogue_item {
  char name[MAX_ITEMS_NAME_LEN];
  float price;
  unsigned int stock;
};

struct catalogue_item catalogue[] = {
    {"bolt", 10.0f, 2}, {"nut", 5.0f, 7},    {"screw", 7.0f, 8},
    {"nail", 3.0f, 7},  {"washer", 2.0f, 3},
};

#define CATALOGUE_SIZE sizeof(catalogue) / sizeof(struct catalogue_item)

unsigned int orders[CATALOGUE_SIZE];

float balance = 200.0f;

int pipe_fd = -1;

void client_error_occured() {
  printf("An error occured, please try again later\n");
  exit(EXIT_FAILURE);
}

void print_welcome_banner() {
  printf("                   ,----,\n");
  printf("                 ,/   .`|                                   ,--.\n");
  printf("  .--.--.      ,`   .'  :,-.----.      ,---,              ,--.'|  "
         ",----..       ,---,.\n");
  printf(" /  /    '.  ;    ;     /\\    /  \\    '  .' \\         ,--,:  : | "
         "/   /   \\    ,'  .' |\n");
  printf("|  :  /`. /.'___,/    ,' ;   :    \\  /  ;    '.    ,`--.'`|  ' :|   "
         ":     : ,---.'   |\n");
  printf(";  |  |--` |    :     |  |   | .\\ : :  :       \\   |   :  :  | |.  "
         " |  ;. / |   |   .'\n");
  printf("|  :  ;_   ;    |.';  ;  .   : |: | :  |   /\\   \\  :   |   \\ | :. "
         "  ; /--`  :   :  |-,\n");
  printf(" \\  \\    `.`----'  |  |  |   |  \\ : |  :  ' ;.   : |   : '  '; |; "
         "  | ;  __ :   |  ;/|\n");
  printf("  `----.   \\   '   :  ;  |   : .  / |  |  ;/  \\   \'   ' ;.    ;|  "
         " : |.' .'|   :   .'\n");
  printf("  __ \\  \\  |   |   |  '  ;   | |  \\ '  :  | \\  \\ ,'|   | | \\   "
         "|.   | '_.' :|   |  |-,\n");
  printf(" /  /`--'  /   '   :  |  |   | ;\\  \\|  |  '  '--'  '   : |  ; .''  "
         " ; : \\  |'   :  ;/|\n");
  printf("'--'.     /    ;   |.'   :   ' | \\.'|  :  :        |   | '`--'  '   "
         "| '/  .'|   |    \\\n");
  printf("  `--'---'     '---'     :   : :-'  |  | ,'        '   : |      |   "
         ":    /  |   :   .'\n");
  printf("                         |   |.'    `--''          ;   |.'       \\  "
         " \\ .'   |   | ,'\n");
  printf("                         `---'                     '---'          "
         "`---`     `----'\n");
  printf("\n");
  printf("                            ,--,           ,----,                    "
         "                ,----,\n");
  printf("               ,----..   ,---.'|         ,/   .`|        ____        "
         "              ,/   .`|\n");
  printf("    ,---,.    /   /   \\  |   | :       ,`   .'  :      ,'  , `.   "
         ",---,          ,`   .'  :\n");
  printf("  ,'  .'  \\  /   .     : :   : |     ;    ;     /   ,-+-,.' _ |  '  "
         ".' \\       ;    ;     /\n");
  printf(",---.' .' | .   /   ;.  \\|   ' :   .'___,/    ,' ,-+-. ;   , || /  "
         ";    '.   .'___,/    ,'\n");
  printf("|   |  |: |.   ;   /  ` ;;   ; '   |    :     | ,--.'|'   |  ;|:  :  "
         "     \\  |    :     |\n");
  printf(":   :  :  /;   |  ; \\ ; |'   | |__ ;    |.';  ;|   |  ,', |  '::  | "
         "  /\\   \\ ;    |.';  ;\n");
  printf(":   |    ; |   :  | ; | '|   | :.'|`----'  |  ||   | /  | |  |||  :  "
         "' ;.   :`----'  |  |\n");
  printf("|   :     \\.   |  ' ' ' :'   :    ;    '   :  ;'   | :  | :  |,|  | "
         " ;/  \\   \\   '   :  ;\n");
  printf("|   |   . |'   ;  \\; /  ||   |  ./     |   |  ';   . |  ; |--' '  : "
         " | \\  \\ ,'   |   |  '\n");
  printf("'   :  '; | \\   \\  ',  / ;   : ;       '   :  ||   : |  | ,    |  "
         "|  '  '--'     '   :  |\n");
  printf("|   |  | ;   ;   :    /  |   ,/        ;   |.' |   : '  |/     |  :  "
         ":           ;   |.'\n");
  printf("|   :   /     \\   \\ .'   '---'         '---'   ;   | |`-'      |  "
         "| ,'           '---'\n");
  printf(
      "|   | ,'       `---`                           |   ;/          `--''\n");
  printf("`----'                                         '---'\n");
  printf("\n");
  printf("Welcome to the new era of automated dispensing machines: Boltmat!\n");
  printf("Here you can buy anything we sell, but remember to keep an eye to "
         "your balance!\n");
}

void read_from_stdin_and_collect_data(char *const buff,
                                      const size_t buff_size) {
  memset(buff, 0, IN_BUF_SIZE);
  if (!fgets(buff, buff_size, stdin)) {
    printf("Error reading from stdin\n");
    exit(EXIT_FAILURE);
  }

  buff[strcspn(buff, "\n")] = 0;

  // Collect data for research purposes :)
  int buff_len = strlen(buff);
  if (write(pipe_fd, buff, buff_len) != buff_len) {
    exit(EXIT_FAILURE);
  }
}

void print_choice_menu() {
  printf("\nWhat would you like to do?\n");
  printf("1. Buy something\n");
  printf("2. Show balance\n");
  printf("3. View cart\n");
  printf("4. Ship order\n");
  printf("0. Logout and lose changes\n");
}

void buy_something() {
  printf("What would you like to buy?\n");
  for (unsigned int i = 0; i < CATALOGUE_SIZE; i++) {
    printf("%s - %.2f₿ - in stock: %d\n", catalogue[i].name, catalogue[i].price,
           catalogue[i].stock);
  }
  printf("quit - Go back\n");

  printf("Please, type your choice: ");
  char buff[IN_BUF_SIZE];
  read_from_stdin_and_collect_data(buff, IN_BUF_SIZE);

  if (strcmp(buff, "quit") == 0)
    return;

  int choice = -1;
  for (unsigned int i = 0; i < CATALOGUE_SIZE; i++)
    if (strcmp(buff, catalogue[i].name) == 0)
      choice = i;

  // user is dumb
  if (choice < 0 || choice >= CATALOGUE_SIZE) {
    printf("Invalid choice\n");
    return;
  }

  printf("How many \'%s\' would you like to buy?\n", catalogue[choice].name);

  read_from_stdin_and_collect_data(buff, IN_BUF_SIZE);

  int quantity = atoi(buff);

  // not enough stock
  if (catalogue[choice].stock < quantity) {
    printf("Sorry, we are out of stock for \'%s\'\n", catalogue[choice].name);
    return;
  }
  // invalid quantity
  if (quantity <= 0) {
    printf("Invalid quantity\n");
    return;
  }

  float total_price = catalogue[choice].price * quantity;
  // not enought money
  if (total_price > balance) {
    printf("You don't have enough balance to buy %d \'%s\'\n", quantity,
           catalogue[choice].name);
    return;
  }
  orders[choice] += quantity;
  catalogue[choice].stock -= quantity;
  balance -= total_price;
  printf("You have added %d \'%s\' to your cart\n", quantity,
         catalogue[choice].name);
}

void show_balace() { printf("Your balance is %.2f₿\n", balance); }

unsigned char is_cart_empty() {
  for (unsigned int i = 0; i < CATALOGUE_SIZE; i++)
    if (orders[i] > 0)
      return 1;
  return 0;
}

void view_cart() {
  if (is_cart_empty() == 0) {
    printf("Your cart looks empty!\n");
    return;
  }

  printf("Your cart:\n");
  for (unsigned int i = 0; i < CATALOGUE_SIZE; i++)
    if (orders[i] > 0) {
      printf("%d. %s - %.2f₿ - quantity: %d\n", i + 1, catalogue[i].name,
             orders[i] * catalogue[i].price, orders[i]);
    }
}

void boltmat_cli(int pipe_fd_write) {
  pipe_fd = pipe_fd_write;

  // error occured in the client, abort
  signal(SIGUSR1, client_error_occured);

  print_welcome_banner();
  char buff[IN_BUF_SIZE];

  while (1) {
    print_choice_menu();
    read_from_stdin_and_collect_data(buff, IN_BUF_SIZE);

    switch (buff[0]) {
    case '1':
      buy_something();
      break;
    case '2':
      show_balace();
      break;
    case '3':
      view_cart();
      break;
    case '0':
      printf("Goodbye!\n");
      return;
    default:
      printf("Invalid choice\n");
      break;
    }
  }
}
