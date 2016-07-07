Dependencies:
   Ubuntu 14.04 LTS X64
   Qt 5.0+
   MySQL 5.7+

How to Compile and Run:
0. Install Development Tool:
   sudo apt-get install build-essential
1. Install Qt5 and MySQL Driver:
   sudo apt-get install qt5-default qttools5-dev-tools libqt5sql5-mysql
2. Install OpenGL Libraries:
   sudo apt-get install mesa-common-dev
   sudo apt-get install libglu1-mesa-dev -y
3. Install MySQL 5.7 Through MySQL Offical APT Repository
   Please Follow section "Steps for a Fresh Installation of MySQL" on Offical MySQL Website:
   http://dev.mysql.com/doc/mysql-apt-repo-quick-guide/en/
4. Create Build Directory (Please create build directory outside the source files directory)
   mkdir build
5. Change Directory to build directory
   cd build
6. Call qmake to generate makefile (Give qmake a path to library_management.pro file as first parameter)
   qmake ../library_management/library_management.pro
7. Call make to build whole project
   make
8. Import Data to Database
   cd data_source
   mysql -u root -p -v -e "source create_database_import_data.sql;"
9. Run program
   cd build
   ./library_management

References:
   http://doc.qt.io/qt-5/linux.html
   https://wiki.qt.io/Install_Qt_5_on_Ubuntu
   http://dev.mysql.com/doc/mysql-apt-repo-quick-guide/en/
