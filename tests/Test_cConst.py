import unittest
import regarch_wrapper as regarch

class TestConst(unittest.TestCase):
    def setUp(self):
        # Create a constant mean model with value 3.5
        self.const_value = 3.5
        self.const = regarch.cConst(self.const_value)
        
    def test_constructor(self):
        """Test constructors"""
        # Default constructor
        default_const = regarch.cConst()
        self.assertEqual(default_const.get_value_index(0, 0), 0.0)
        
        # Constructor with value
        self.assertEqual(self.const.get_value_index(0, 0), self.const_value)
        
        # Copy constructor
        copy_const = regarch.cConst(self.const)
        self.assertEqual(copy_const.get_value_index(0, 0), self.const_value)
        
    def test_set_get(self):
        """Test setting and getting values"""
        # Set to a new value
        new_value = 7.5
        self.const.set_value_index(new_value, 0, 0)
        self.assertEqual(self.const.get_value_index(0, 0), new_value)
        
        # Using named access
        self.const.set_value_name(self.const_value, "Const")  # Reset to original
        self.assertEqual(self.const.get_value_name("Const", 0), self.const_value)
        
    def test_compute_mean(self):
        """Test computing the conditional mean"""
        # Create data object
        data = regarch.cRegArchValue(10)  # 10 data points
        
        # Initialize data values
        for i in range(10):
            data.mYt[i] = float(i)
            
        # For constant mean, the result should always be the constant value
        for date in range(10):
            mean = self.const.compute_mean(date, data)
            self.assertEqual(mean, self.const_value)
            
    def test_assignment(self):
        """Test assignment operator"""
        # Create a new constant mean with different value
        other_const = regarch.cConst(9.9)
        
        # Assign our original to it
        other_const.__assign__(self.const)
        
        # Check that values match
        self.assertEqual(other_const.get_value_index(0, 0), self.const_value)

if __name__ == '__main__':
    unittest.main()