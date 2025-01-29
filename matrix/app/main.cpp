#include "matrix.h"
#include "coo.h"
#include "csr.h"
#include <cassert>
#include <array>
#include <utility>

int main()
{
    matrix<int> m(-1);

    assert(m.size() == 0); // все ячейки свободны
    auto a = m[0][0];
    assert(a == -1);
    assert(m.size() == 0);
    m[100][100] = 314;
    assert(m[100][100] == 314);
    assert(m.size() == 1);

    for (auto c : m)
    {
        int x;
        int y;
        int v;
        std::tie(x, y, v) = c;
        std::cout << x << y << v << std::endl;
    }

    // При запуске программы необходимо создать матрицу с пустым значением 0, заполнить главную диагональ матрицы (от [0,0] до [9,9]) значениями от 0 до 9.
    matrix<int> Matrix;
    for (int i = 0; i < 10; i++)
    {
        Matrix[i][i] = i;
    }
    // Второстепенную диагональ (от [0,9] до [9,0]) значениями от 9 до 0.
    for (int i = 0; i < 10; i++)
    {
        Matrix[i][9 - i] = i;
    }
    // Необходимо вывести фрагмент матрицы от [1,1] до [8,8]. Между столбцами пробел. Каждая строка матрицы на новой строке консоли.
    for (int i = 1; i < 9; i++)
    {
        for (int j = 1; j < 9; j++)
        {
            std::cout << Matrix[i][j] << ' ';
        }
        std::cout << std::endl;
    }

    // Вывести количество занятых ячеек.
    std::cout << "Cell count: " << Matrix.size() << std::endl;

    // Вывести все занятые ячейки вместе со своими позициями.
    int cellNum = 1;
    for (const auto &[row, col, value] : Matrix)
    {
        std::cout << "Element " << cellNum++ << " at (" << row << ", " << col << ") = " << value << "\n";
    }

    // Опционально реализовать N-мерную матрицу.
    // Размер задается строго и не меняется. способ хранения элементов другой.

    constexpr std::size_t matrixSize = 3;
    constexpr auto d = makeArray(std::make_index_sequence<matrixSize>{}); // просто баловство.
    // Используем std::views::iota вместо массива
    // constexpr auto indices = std::views::iota(0uz, matrixSize);

    matrix<double, SparseMatrixCSR<double>> m1(matrixSize, matrixSize, -1.0);

    for (auto i :d)
        m1[i][i] = i;

    for (auto i : d)
    {
        for (auto j : d)
            std::cout << Matrix[i][j] << ' ';
        std::cout << std::endl;
    }

    // Опционально реализовать каноническую форму оператора =, допускающую выражения
    //((Matrix[100][100] = 314) = 0) = 217
    ((Matrix[100][100] = 314) = 0) = 217;
    std::cout << "((Matrix[100][100] = 314) = 0) = 217: " << Matrix[100][100] << std::endl;

    return 0;
}