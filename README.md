# commands :
```
git clone https://github.com/Saranya625/CD_project.git
sudo apt-get install flex bison gcc
make
 gcc lex.yy.c parser.tab.c semantic.c ir.c riscv.c -o compiler -lfl

./compiler < test_bubble_sort.cd
```

//the part i need to use//

 ./compiler < riscv_test/test_min.cd

//later this step output.ir is generated

 ./riscv_driver
  cat output.s


