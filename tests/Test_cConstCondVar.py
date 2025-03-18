import unittest
import regarch_wrapper as regarch

class TestConstCondVar(unittest.TestCase):
    def setUp(self):
        # Create a constant variance model with value 2.5
        self.const_value = 2.5
        self.const_var = regarch.cConstCondVar(self.const_value)
        
    def test_constructor(self):
        """Test constructors"""
        # Default constructor
        default_var = regarch.cConstCondVar()
        self.assertEqual(default_var.get_value_index(0, 0), 1.0)  # Default should be 1.0
        
        # Constructor with value
        self.assertEqual(self.const_var.get_value_index(0, 0), self.const_value)
        
        # Copy constructor
        copy_var = regarch.cConstCondVar(self.const_var)
        self.assertEqual(copy_var.get_value_index(0, 0), self.const_value)
        
    def test_set_get(self):
        """Test setting and getting values"""
        # Set to a new value
        new_value = 4.5
        self.const_var.set_value_index(new_value, 0, 0)
        self.assertEqual(self.const_var.get_value_index(0, 0), new_value)
        
        # Using named access
        self.const_var.set_value_name(self.const_value, "ConstVar")  # Reset to original
        self.assertEqual(self.const_var.get_value_name("ConstVar", 0), self.const_value)
        
    def test_compute_var(self):
        """Test computing the conditional variance"""
        # Create data object
        data = regarch.cRegArchValue(10)  # 10 data points
        
        # Initialize data values
        for i in range(10):
            data.mYt[i] = float(i)
            
        # For constant variance, the result should always be the constant value
        for date in range(10):
            var = self.const_var.compute_var(date, data)
            self.assertEqual(var, self.const_value)
            
    def test_assignment(self):
        """Test assignment operator"""
        # Create a new constant variance with different value
        other_var = regarch.cConstCondVar(9.9)
        
        # Assign our original to it
        other_var.__assign__(self.const_var)
        
        # Check that values match
        self.assertEqual(other_var.get_value_index(0, 0), self.const_value)

if __name__ == '__main__':
    unittest.main()