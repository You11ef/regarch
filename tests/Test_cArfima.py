import unittest
import numpy as np
import sys

# Import the regarch module
try:
    import regarch_wrapper as regarch
    print("Successfully imported regarch module")
except ImportError:
    print("RegArch module not found. Make sure it's in your Python path.")
    sys.exit(1)

class TestArfima(unittest.TestCase):
    """
    Test cases for the cArfima Python wrapper
    """
    
    def setUp(self):
        """Set up test fixtures"""
        # Create a simple ARFIMA(1,d,1) model
        self.ar_coef = 0.7
        self.ma_coef = -0.3
        self.frac_d = 0.2
        self.arfima = regarch.cArfima(1, 1, self.frac_d)
        
        # Set AR and MA coefficients
        self.arfima.set_value_index(self.ar_coef, 0, 0)  # First AR coefficient
        self.arfima.set_value_index(self.ma_coef, 0, 1)  # First MA coefficient

    def test_constructor_default(self):
        """Test default constructor"""
        model = regarch.cArfima()
        self.assertEqual(model.get_n_ar(), 0)
        self.assertEqual(model.get_n_ma(), 0)
        self.assertEqual(model.get_n_param(), 1)  # Just d parameter

    def test_constructor_parameters(self):
        """Test constructor with parameters"""
        model = regarch.cArfima(2, 3, 0.4)
        self.assertEqual(model.get_n_ar(), 2)
        self.assertEqual(model.get_n_ma(), 3)
        self.assertEqual(model.get_n_param(), 6)  # 2 AR + 3 MA + 1 d

    def test_set_get_value_index(self):
        """Test setting and getting values by index"""
        # Values already set in setUp
        self.assertAlmostEqual(self.arfima.get_value_index(0, 0), self.ar_coef)
        self.assertAlmostEqual(self.arfima.get_value_index(0, 1), self.ma_coef)
        self.assertAlmostEqual(self.arfima.get_value_index(0, 2), self.frac_d)

    def test_resize_poly(self):
        """Test resizing polynomial representations"""
        try:
            self.arfima.resize_poly(3)  # Should be a Python-style alias
            # No assert - just checking that it doesn't throw an exception
        except AttributeError:
            try:
                # Try the original name if the Python style name is not available
                self.arfima.ReSizePoly(3)
            except Exception as e:
                self.fail(f"ReSizePoly raised exception: {e}")
        except Exception as e:
            self.fail(f"resize_poly raised exception: {e}")

    def test_reg_arch_param_to_vector(self):
        """Test converting parameters to vector representation"""
        vec = regarch.cGSLVector(3)  # AR + MA + d = 3 parameters
        try:
            self.arfima.reg_arch_param_to_vector(vec, 0)
            self.assertEqual(vec.GetSize(), 3)
            self.assertAlmostEqual(vec[0], self.ar_coef)
            self.assertAlmostEqual(vec[1], self.ma_coef)
            self.assertAlmostEqual(vec[2], self.frac_d)
        except AttributeError:
            try:
                # Try the original name if the Python style name is not available
                self.arfima.RegArchParamToVector(vec, 0)
                self.assertEqual(vec.GetSize(), 3)
                self.assertAlmostEqual(vec[0], self.ar_coef)
                self.assertAlmostEqual(vec[1], self.ma_coef)
                self.assertAlmostEqual(vec[2], self.frac_d)
            except Exception as e:
                self.fail(f"RegArchParamToVector raised exception: {e}")
        except Exception as e:
            self.fail(f"reg_arch_param_to_vector raised exception: {e}")

    def test_vector_to_reg_arch_param(self):
        """Test converting from vector representation to parameters"""
        vec = regarch.cGSLVector(3)
        vec[0] = 0.5
        vec[1] = -0.2
        vec[2] = 0.3
        
        try:
            self.arfima.vector_to_reg_arch_param(vec)
            self.assertAlmostEqual(self.arfima.get_value_index(0, 0), 0.5)
            self.assertAlmostEqual(self.arfima.get_value_index(0, 1), -0.2)
            self.assertAlmostEqual(self.arfima.get_value_index(0, 2), 0.3)
        except AttributeError:
            try:
                # Try the original name if the Python style name is not available
                self.arfima.VectorToRegArchParam(vec)
                self.assertAlmostEqual(self.arfima.get_value_index(0, 0), 0.5)
                self.assertAlmostEqual(self.arfima.get_value_index(0, 1), -0.2)
                self.assertAlmostEqual(self.arfima.get_value_index(0, 2), 0.3)
            except Exception as e:
                self.fail(f"VectorToRegArchParam raised exception: {e}")
        except Exception as e:
            self.fail(f"vector_to_reg_arch_param raised exception: {e}")

if __name__ == '__main__':
    unittest.main()