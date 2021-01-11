# cpp-file-system  
Файлова система - проект по СДП.  

# Описание на подържаните команди
```
MANUAL
# man
Prints this text

# paths
<path>
/abs/path
rel/path
Supports use of . and ..
Supports symlinks.

# pwd
Print working directory

# cd <path_to_dir>
Change current working directory.

# ls <path_to_dir>
Print directory

# cat <path_to_file>[*n] [>] [path_to_output]
Concatenate files and paste them onto output file.

# cp <path_to_file>[*n] <path_to_dir>
Copy files to directory.

# rm <path_to_file>[*n]
Remove files.

# mkdir <path_to_dir>[*n]
Create one or more directories.

# rmdir <path_to_dor>[*n]
Remove one or more directories.

# ln <path_to_file> <new_name>
Create a symlink to a file with a given path and new name.

# stat <path_to_file> [*n]
Print metadata information for files.

```
# Клас Tree
Реализиран е шаблонен клас Tree\<T\>, чрез навързано представяне. <br>
Tree\<T\> пази указател към корен от тип Tree\<T\>::Node. <br>
Tree\<T\>::Node пази данните в един връх на дървото, децата и родителят на този връх. <br>
Децата и родителят на този връх са от тип Tree\<T\>::Node* <br>
Класът се ползва за да се реализира файлова система, която е със дърворидна структура. <br>


Сам по себе си Tree няма много функционалност. <br>
Функции за достъп до дървото са предоставени в Tree::TreeIterator. <br>

Tree\<T\>::begin връща TreeIterator <br>

# Клас Tree::Iterator
Промени по Tree могат да бъдат направени с Tree::Iterator. <br>
Функции за достъп: T getCur(), T& getChild(), int childrenCount(), bool hasParent() <br>
Функции за добавяне/ премахване на върхове: void removeCur(), void addChild(T added)  <br>
Функции за преместване на курсура: void moveToChild(int i), void moreToParent() <br>

# Клас FileSystemNode
Файл от файловата система. <br>
Пази метаданните за файла, както и неговото съдържание.
# Клас FileSystem
Файлова система, която е реализирана чрез Tree\<FileSystemNode\>. <br>
Текущата работеща директория се пази с Tree\<FileSystemNode\>::TreeIterator. <br>
  <br><br>
Подърржа се :създаване на празна файлова система, зареждане на файлова система от файл, зареждане на файлова върху празна такава.

# Клас FileSystemCLI
Пази в себе си указател към FileSystem. <br>
Може да бъде създаден FileSystemCLI, без външна инстанция на FileSystem. Тогава FileSystemClI си създава собствена инстанция. <br>
Използва се за обработка на FileSystem, чрез текстови команди. <br>

# Конфигурация
Възможно е конфигуриране на различни символни и текстови константи във файловата система. Това може да стане чрез променяне на една от следните константи:
```
const char PATH_ROOT_SYM = '/';
const char PATH_DIR_SEP = '/';
const string PATH_UP_STR = "..";
const string PATH_STAY_STR = ".";
const string FILE_TYPE_SEP = ".";
const int NODE_METADATA_BEGIN = 1;
const int NODE_METADATA_SIZE = 9;
const string NODE_PARENT_INDEX_BEGIN_FLAG = "PARENT_INDEX_ARRAY:";
const string DIRECTORY_TYPE_STR = "dir";
const string CAT_REDIRECT_STR = ">";
const char END_OF_INPUT_CH = '.';
const string END_OF_INPUT_STR = ".";
const string SYMLINK_TYPE_STR = "symlink";

const string MAN_STR;
```
# Зареждане от файл
Поддържа се зареждане от файл със специален формат:
```
FILE_METADATA_NODES:
n*[<id>
<size (0 ако isDir=1)>
<isDir (1/0)>
<name>
<type (symlink/dir)>
<created>
<lastChange>
<contents (=X, ако isDir=1)>]
##########
PARENT INDEX ARRAY:
[parent index]*n
```
Пример за файлова система:
```
FILE_METADATA_NODES:
0
0
1
/
dir
0
0
X
#############
1
0
1
folder1
dir
0
0
X
#############
2
0
1
folder2
dir
0
0
X
#############
3
20
0
file1
txt
0
0
20 symbols long file.
#############
4
10
0
file2
txt
0
0
10 symbols.
#############
5
5
0
file3
txt
0
0
7 syms.
#############
6
5
0
file4
txt
0
0
5 sym
#############
7
0
1
folder3
dir
0
0
X
#############
8
5
0
file5
txt
0
0
5 sym
#############
9
5
0
file6
txt
0
0
5 sym
#############
10
5
0
sl1
symlink
0
0
/folder1/folder3/file6.txt
PARENT_INDEX_ARRAY:
-1, 0, 0, 0, 2, 2, 1, 1, 7, 7, 0
 0  1  2  3  4  5  6  7  8  9 10
```

#  Реализация на symlink и директории
Всички директории са FileSystemNode с type == dir и isDir == true.
Командата ln, създава файл от тип symlink. Като съдържание, той пази абсолютният път до оригиналния файл.
