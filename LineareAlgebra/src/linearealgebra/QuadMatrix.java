/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package linearealgebra;

import java.util.Arrays;

/**
 *
 * @author fabian
 */
public class QuadMatrix {
    
    private double[][] matrix;
    
    public QuadMatrix(double[][] matrix) {
        
        for (double [] line : matrix) {
            if (line.length != matrix.length)
                throw new IllegalArgumentException("Matrix ist nicht quadratisch");
        }
        
        this.matrix = new double[matrix.length][matrix.length];
        for (int i = 0; i < matrix.length;++i)
        {
            for (int j = 0; j < matrix.length;++j)
                this.matrix[i][j] = matrix[i][j];
        }
    }
    
    public QuadMatrix(QuadMatrix qm) {
        this(qm.matrix);
    }
    
    public int getDim() {
        return matrix.length;
    }
    
    public String toString() {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < matrix.length; ++i)
            sb.append(Arrays.toString(matrix[i])+"\n");
        return sb.toString();
    }
    
    public QuadMatrix addiere (QuadMatrix qm)
    {
        QuadMatrix result = new QuadMatrix(qm);
        if (this.getDim() != result.getDim())
            throw new IllegalArgumentException("Dimensionen stimmen nicht überein");
        
        for (int i = 0; i < matrix.length;++i)
        {
            for (int j = 0; j < matrix.length;++j)
                result.matrix[i][j] += this.matrix[i][j];
        }
        return result;
    }
    
    public QuadMatrix multipliziere(double lambda){
       QuadMatrix result = new QuadMatrix(this);
        for (int i = 0; i < matrix.length;++i)
        {
            for (int j = 0; j < matrix.length;++j)
                result.matrix[i][j] *= lambda;
        }
        return result; 
    }
}
