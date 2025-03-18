import unittest
import numpy as np
from regarch_wrapper import cGSLVector, cGSLMatrix
from regarch_wrapper import Norm, Sum, Maxi, Mini, Abs, IsNaN

class TestCGSLVector(unittest.TestCase):
    
    def setUp(self):
        # Create vectors for testing
        self.v1 = cGSLVector(3, 1.0)  # [1.0, 1.0, 1.0]
        self.v2 = cGSLVector(3, 2.0)  # [2.0, 2.0, 2.0]
        
    def test_constructor_default(self):
        """Test default constructor"""
        v = cGSLVector()
        self.assertEqual(v.GetSize(), 0)
    
    def test_constructor_size(self):
        """Test size constructor with default value"""
        v = cGSLVector(5)
        self.assertEqual(v.GetSize(), 5)
        self.assertEqual(v[0], 0.0)
        self.assertEqual(v[4], 0.0)
    
    def test_constructor_size_value(self):
        """Test size and value constructor"""
        v = cGSLVector(3, 7.5)
        self.assertEqual(v.GetSize(), 3)
        self.assertEqual(v[0], 7.5)
        self.assertEqual(v[1], 7.5)
        self.assertEqual(v[2], 7.5)
    
    def test_indexing(self):
        """Test indexing (get and set)"""
        v = cGSLVector(3, 7.5)
        
        # Test getting values
        self.assertEqual(v[0], 7.5)
        self.assertEqual(v[1], 7.5)
        self.assertEqual(v[2], 7.5)
        
        # Test setting values via indexing
        v[1] = 10.0
        self.assertEqual(v[0], 7.5)
        self.assertEqual(v[1], 10.0)
        self.assertEqual(v[2], 7.5)
        
        # Test setting values via Set method
        v.Set(15.0, 2)
        self.assertEqual(v[0], 7.5)
        self.assertEqual(v[1], 10.0)
        self.assertEqual(v[2], 15.0)
    
    def test_realloc_size_value(self):
        """Test ReAlloc with size and value"""
        v = cGSLVector()
        v.ReAlloc(4, 3.0)
        self.assertEqual(v.GetSize(), 4)
        self.assertEqual(v[0], 3.0)
        self.assertEqual(v[3], 3.0)
    
    def test_realloc_vector(self):
        """Test ReAlloc with another vector"""
        v = cGSLVector()
        v.ReAlloc(self.v1)
        self.assertEqual(v.GetSize(), 3)
        self.assertEqual(v[0], 1.0)
        self.assertEqual(v[2], 1.0)
    
    def test_assign_double(self):
        """Test AssignDouble (operator= double)"""
        v = cGSLVector(3)
        v.AssignDouble(4.5)
        self.assertEqual(v[0], 4.5)
        self.assertEqual(v[1], 4.5)
        self.assertEqual(v[2], 4.5)
    
    def test_assign_vector(self):
        """Test AssignVector (operator= vector)"""
        v = cGSLVector(3)
        v.AssignVector(self.v1)
        self.assertEqual(v[0], 1.0)
        self.assertEqual(v[1], 1.0)
        self.assertEqual(v[2], 1.0)
    
    def test_assign_matrix(self):
        """Test AssignMatrix (operator= matrix)"""
        m = cGSLMatrix(3, 1, 7.0)  # 3x1 matrix filled with 7.0
        v = cGSLVector(3)
        v.AssignMatrix(m)
        self.assertEqual(v[0], 7.0)
        self.assertEqual(v[1], 7.0)
        self.assertEqual(v[2], 7.0)
    
    def test_add_double_in_place(self):
        """Test AddDoubleInPlace (operator+= double)"""
        v = cGSLVector(3, 1.0)
        v.AddDoubleInPlace(2.0)  # v = [3.0, 3.0, 3.0]
        self.assertEqual(v[0], 3.0)
        self.assertEqual(v[1], 3.0)
        self.assertEqual(v[2], 3.0)
    
    def test_add_vector_in_place(self):
        """Test AddVectorInPlace (operator+= vector)"""
        v = cGSLVector(3, 1.0)
        v.AddVectorInPlace(self.v2)  # v = [3.0, 3.0, 3.0]
        self.assertEqual(v[0], 3.0)
        self.assertEqual(v[1], 3.0)
        self.assertEqual(v[2], 3.0)
    
    def test_subtract_double_in_place(self):
        """Test SubtractDoubleInPlace (operator-= double)"""
        v = cGSLVector(3, 5.0)
        v.SubtractDoubleInPlace(2.0)  # v = [3.0, 3.0, 3.0]
        self.assertEqual(v[0], 3.0)
        self.assertEqual(v[1], 3.0)
        self.assertEqual(v[2], 3.0)
    
    def test_subtract_vector_in_place(self):
        """Test SubtractVectorInPlace (operator-= vector)"""
        v = cGSLVector(3, 3.0)
        v.SubtractVectorInPlace(self.v1)  # v = [2.0, 2.0, 2.0]
        self.assertEqual(v[0], 2.0)
        self.assertEqual(v[1], 2.0)
        self.assertEqual(v[2], 2.0)
    
    def test_multiply_in_place(self):
        """Test MultiplyInPlace (operator*= double)"""
        v = cGSLVector(3, 2.0)
        v.MultiplyInPlace(3.0)  # v = [6.0, 6.0, 6.0]
        self.assertEqual(v[0], 6.0)
        self.assertEqual(v[1], 6.0)
        self.assertEqual(v[2], 6.0)
    
    def test_divide_in_place(self):
        """Test DivideInPlace (operator/= double)"""
        v = cGSLVector(3, 6.0)
        v.DivideInPlace(3.0)  # v = [2.0, 2.0, 2.0]
        self.assertEqual(v[0], 2.0)
        self.assertEqual(v[1], 2.0)
        self.assertEqual(v[2], 2.0)
    
    def test_add_double_left(self):
        """Test AddDoubleLeft (double + vector)"""
        result = cGSLVector(3, 0.0)
        
        # Test through the Python wrapper function
        from regarch_wrapper import AddDoubleLeft
        result = AddDoubleLeft(2.0, self.v1)  # 2.0 + [1.0, 1.0, 1.0] = [3.0, 3.0, 3.0]
        self.assertEqual(result[0], 3.0)
        self.assertEqual(result[1], 3.0)
        self.assertEqual(result[2], 3.0)
    
    def test_add_double_right(self):
        """Test AddDoubleRight (vector + double)"""
        from regarch_wrapper import AddDoubleRight
        result = AddDoubleRight(self.v1, 2.0)  # [1.0, 1.0, 1.0] + 2.0 = [3.0, 3.0, 3.0]
        self.assertEqual(result[0], 3.0)
        self.assertEqual(result[1], 3.0)
        self.assertEqual(result[2], 3.0)
    
    def test_norm(self):
        """Test Norm function"""
        # Create vector [3.0, 4.0]
        v = cGSLVector(2)
        v[0] = 3.0
        v[1] = 4.0
        
        # Expected norm is 5.0 (Pythagorean triple)
        self.assertAlmostEqual(Norm(v), 5.0)
    
    def test_sum(self):
        """Test Sum function"""
        # Create vector [1.0, 2.0, 3.0]
        v = cGSLVector(3)
        v[0] = 1.0
        v[1] = 2.0
        v[2] = 3.0
        
        # Expected sum is 6.0
        self.assertEqual(Sum(v), 6.0)
    
    def test_maxi_mini(self):
        """Test Maxi and Mini functions"""
        # Create vector [1.0, 5.0, 3.0]
        v = cGSLVector(3)
        v[0] = 1.0
        v[1] = 5.0
        v[2] = 3.0
        
        # Expected max is 5.0, min is 1.0
        self.assertEqual(Maxi(v), 5.0)
        self.assertEqual(Mini(v), 1.0)
    
    def test_abs(self):
        """Test Abs function"""
        # Create vector [-1.0, 2.0, -3.0]
        v = cGSLVector(3)
        v[0] = -1.0
        v[1] = 2.0
        v[2] = -3.0
        
        # Get absolute values
        result = Abs(v)
        
        # Expected result is [1.0,, 2.0, 3.0]
        self.assertEqual(result[0], 1.0)
        self.assertEqual(result[1], 2.0)
        self.assertEqual(result[2], 3.0)
    
    def test_is_nan(self):
        """Test IsNaN function"""
        # Create a normal vector
        v1 = cGSLVector(2, 1.0)
        self.assertFalse(IsNaN(v1))
        
        # Create a vector with NaN
        v2 = cGSLVector(2, 1.0)
        v2[1] = float('nan')
        self.assertTrue(IsNaN(v2))

if __name__ == '__main__':
    unittest.main()