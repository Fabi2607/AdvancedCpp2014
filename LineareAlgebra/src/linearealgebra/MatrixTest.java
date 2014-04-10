/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package linearealgebra;

/**
 *
 * @author fabian
 */
public class MatrixTest {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        double[][] matrixA = {{1, 2, 3}, {1, 1, 2}, {3, 2, 2}};
        double[][] matrixB = {{2, -1, 1}, {1, -2, 0}, {0, 3, 1}};

        double lambda = 5;
        QuadMatrix a = new QuadMatrix(matrixA);
        QuadMatrix b = new QuadMatrix(matrixB);
        System.out.println("Dimension: " + a.getDim());
        System.out.println(a.addiere(b));
        System.out.println(a.multipliziere(lambda));
    }

}
