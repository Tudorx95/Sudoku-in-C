#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define N 4

typedef struct BOX
{
      int lines, cols;
      int start_line, start_col;
} BOX;

typedef struct partialSolution
{
      int sudokuMatrix[N][N];
      int row;
      int column;
      BOX *box;
      short int find;
} partialSolution;

int find_minimum_square(int n)
{
      int i;
      for (i = 1; i * i < n; i++)
            ;
      if (i * i == n)
            return i;
      else
            return i - 1;
}

partialSolution readSudoku(const char *filename)
{
      partialSolution ps;
      FILE *f = fopen(filename, "r");
      if (feof(f))
            return ps;
      if (f == NULL)
            return ps;

      for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
            {
                  fscanf(f, "%d \n", &ps.sudokuMatrix[i][j]);
            }
      fclose(f);
      ps.box = (BOX *)malloc(sizeof(BOX));
      ps.row = ps.column = -1;
      ps.box->lines = find_minimum_square(N);
      ps.box->cols = (ps.box->lines * ps.box->lines == N ? ps.box->lines : ps.box->lines + 1);
      ps.box->start_line = ps.box->start_col = 0;
      return ps;
}

void printSudoku(partialSolution ps)
{

      for (int i = 0; i < N; i++)
      {
            for (int j = 0; j < N; j++)
                  if (ps.sudokuMatrix[i][j] < 10)
                        printf("%d ", ps.sudokuMatrix[i][j]);
                  else // modalitate speciala de afisare pentru matricile mai mari de 9x9
                        printf("%c ", 'A' + ps.sudokuMatrix[i][j] % 10);
            printf("\n");
      }
      printf("\n");
}

int line_col_validation(partialSolution PS)
{
      int find1 = 0, find2 = 0;
      for (int i = 0; i < N; i++)
      {
            if (PS.sudokuMatrix[PS.row][i] && PS.sudokuMatrix[PS.row][i] == PS.sudokuMatrix[PS.row][PS.column])
                  find1++;
            if (PS.sudokuMatrix[i][PS.column] && PS.sudokuMatrix[i][PS.column] == PS.sudokuMatrix[PS.row][PS.column])
                  find2++;
      }
      if (find1 != 1 || find2 != 1)
            return 0;
      return 1;
}

int box_validation(partialSolution PS)
{
      int nr_lines = PS.box->lines, nr_cols = PS.box->cols;
      int a = PS.box->start_line, b = PS.box->start_col;

      int *visited = (int *)calloc((nr_lines * nr_cols + 1), sizeof(int));
      for (int i = a; i < a + nr_lines; i++)
      {
            for (int j = b; j < b + nr_cols; j++)
            {
                  visited[PS.sudokuMatrix[i][j]]++;
                  if (PS.sudokuMatrix[i][j] && visited[PS.sudokuMatrix[i][j]] > 1)
                        return 0;
            }
      }
      free(visited);
      return 1;
}

int canReject(partialSolution PS)
{
      // not all numbers are in each line, column and box
      // verify existence on the line and coloumn
      if (PS.row == -1 || PS.column == -1)
            return 0;
      if (PS.sudokuMatrix[PS.row][PS.column] && line_col_validation(PS) == 0)
            return 1;
      // verify in the box
      if (box_validation(PS) == 0)
            return 1;
      return 0;
}

int isSolution(partialSolution PS)
{
      // check if all boxes are filled and each line, column and box respect the rule(each number just onces)
      for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
            {
                  if (PS.sudokuMatrix[i][j] == 0 || line_col_validation(PS) == 0 || box_validation(PS) == 0)
                        return 0;
            }
      return 1;
}

partialSolution increaseStep(partialSolution PS)
{
      // try filling the next empty box
      if (PS.row == -1 && PS.column == -1)
            PS.row = 0, PS.column = 0;
      // If it reached the end of box position or the position is occupied
      if (PS.sudokuMatrix[PS.row][PS.column] && PS.row == PS.box->start_line + PS.box->lines - 1 && PS.column == PS.box->start_col + PS.box->cols - 1)
      {
            // Increment box positions
            if (PS.box->start_col + PS.box->cols < N)
            {
                  PS.column = PS.box->start_col + PS.box->cols;
                  PS.box->start_col = PS.column;
                  PS.row = PS.box->start_line;
                  PS.box->start_line = PS.row;
            }
            else if (PS.row + PS.box->lines < N)
            {
                  PS.row = PS.row + 1; // PS.box->lines-1;
                  PS.box->start_line = PS.row;
                  PS.column = 0;
                  PS.box->start_col = PS.column;
            }
      }
      return PS;
}

partialSolution getNextChoiceAtStep(partialSolution PS)
{
      int find = 0;
      for (int i = PS.box->start_line, k1 = 0; k1 < PS.box->lines; i++, k1++)
      {
            for (int j = PS.box->start_col, k2 = 0; k2 < PS.box->cols; j++, k2++)
                  if (PS.sudokuMatrix[i][j] == 0)
                  {
                        PS.row = i, PS.column = j;
                        find = 1;
                        return PS;
                  }
      }
      if (find == 0)
            PS.row = PS.box->start_line + PS.box->lines - 1, PS.column = PS.box->start_col + PS.box->cols - 1;
      return PS;
}

int hasChoiceAtStep(partialSolution PS)
{
      for (int i = PS.box->start_line, k1 = 0; k1 < PS.box->lines; i++, k1++)
      {
            for (int j = PS.box->start_col, k2 = 0; k2 < PS.box->cols; j++, k2++)
                  if (PS.sudokuMatrix[i][j] == 0)
                        return 1;
      }
      return 0;
}

void backtracking(partialSolution PS)
{
      if (canReject(PS))
            return;
      if (isSolution(PS))
      {
            printSudoku(PS);
            return;
      }
      // printSudoku(PS);
      PS = getNextChoiceAtStep(PS);
      PS = increaseStep(PS);
      if (hasChoiceAtStep(PS))
      {
            if (PS.sudokuMatrix[PS.row][PS.column])
            {
                  backtracking(PS);
                  // break;
            }
            else
            {
                  for (int i = 1; i <= N; i++)
                  {
                        PS.sudokuMatrix[PS.row][PS.column] = i;
                        int a = PS.row, b = PS.column, c = PS.box->start_line, d = PS.box->start_col;
                        backtracking(PS);
                        PS.row = a, PS.column = b;
                        PS.box->start_line = c, PS.box->start_col = d;
                        PS.sudokuMatrix[PS.row][PS.column] = 0;
                  }
            }
      }
}

int main()
{
      // Atentie la N si la modificarea fisierului!
      partialSolution PS = readSudoku("input.txt");
      PS.find = 0;
      backtracking(PS);

      return 0;
}