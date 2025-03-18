import unittest
import math
from regarch_wrapper import eCondMeanEnum, eCondVarEnum, eDistrTypeEnum
from regarch_wrapper import MIN, MAX, SIGN
from regarch_wrapper import PI, SQRT_PI, SQRT_2_PI, LOG_SQRT_2_PI, LOG_PI
from regarch_wrapper import MAX_COND_MEAN, MAX_COND_VAR

class TestRegArchDef(unittest.TestCase):
    
    def test_enums_existence(self):
        """Test that all enums are defined and have expected values"""
        # Test eCondMeanEnum values
        self.assertEqual(eCondMeanEnum.eUnknown, 0)
        self.assertEqual(eCondMeanEnum.eConst, 1)
        self.assertEqual(eCondMeanEnum.eAr, 2)
        self.assertEqual(eCondMeanEnum.eMa, 3)
        self.assertEqual(eCondMeanEnum.eLinReg, 4)
        self.assertEqual(eCondMeanEnum.eStdDevInMean, 5)
        self.assertEqual(eCondMeanEnum.eVarInMean, 6)
        self.assertEqual(eCondMeanEnum.eArfima, 7)
        
        # Test eCondVarEnum values
        self.assertEqual(eCondVarEnum.eNotKnown, 0)
        self.assertEqual(eCondVarEnum.eCste, 1)
        self.assertEqual(eCondVarEnum.eArch, 2)
        self.assertEqual(eCondVarEnum.eGarch, 3)
        self.assertEqual(eCondVarEnum.eNgarch, 4)
        self.assertEqual(eCondVarEnum.eEgarch, 5)
        self.assertEqual(eCondVarEnum.eAparch, 6)
        self.assertEqual(eCondVarEnum.eTarch, 7)
        self.assertEqual(eCondVarEnum.eFigarch, 8)
        self.assertEqual(eCondVarEnum.eUgarch, 9)
        self.assertEqual(eCondVarEnum.eTsgarch, 10)
        self.assertEqual(eCondVarEnum.eGtarch, 11)
        self.assertEqual(eCondVarEnum.eNagarch, 12)
        self.assertEqual(eCondVarEnum.eSqrgarch, 13)
        
        # Test eDistrTypeEnum values
        self.assertEqual(eDistrTypeEnum.eUndefined, 0)
        self.assertEqual(eDistrTypeEnum.eNormal, 1)
        self.assertEqual(eDistrTypeEnum.eStudent, 2)
        self.assertEqual(eDistrTypeEnum.eGed, 3)
        self.assertEqual(eDistrTypeEnum.eMixNorm, 4)
    
    def test_constants(self):
        """Test that all constants are defined and have expected values"""
        # Verify PI is correct
        self.assertAlmostEqual(PI, 3.141592653589793, places=10)
        
        # Verify SQRT_PI is correct
        self.assertAlmostEqual(SQRT_PI, 1.7724538509055, places=10)
        
        # Verify SQRT_2_PI is correct
        self.assertAlmostEqual(SQRT_2_PI, 2.5066282746310, places=10)
        
        # Verify LOG_SQRT_2_PI is correct
        self.assertAlmostEqual(LOG_SQRT_2_PI, 0.9189385332046, places=10)
        
        # Verify LOG_PI is correct
        self.assertAlmostEqual(LOG_PI, 1.1447298858494, places=10)
        
        # Verify MAX_COND_MEAN is correct
        self.assertEqual(MAX_COND_MEAN, 8)
        
        # Verify MAX_COND_VAR is correct
        self.assertEqual(MAX_COND_VAR, 14)
    
    def test_min_function(self):
        """Test the MIN function"""
        self.assertEqual(MIN(5, 10), 5)
        self.assertEqual(MIN(10, 5), 5)
        self.assertEqual(MIN(-5, 5), -5)
        self.assertEqual(MIN(5, 5), 5)
    
    def test_max_function(self):
        """Test the MAX function"""
        self.assertEqual(MAX(5, 10), 10)
        self.assertEqual(MAX(10, 5), 10)
        self.assertEqual(MAX(-5, 5), 5)
        self.assertEqual(MAX(5, 5), 5)
    
    def test_sign_function(self):
        """Test the SIGN function"""
        self.assertEqual(SIGN(5), 1)
        self.assertEqual(SIGN(-5), -1)
        self.assertEqual(SIGN(0), -1)  # According to the implementation
    
    def test_enum_usage(self):
        """Test using enums for type checking"""
        # Function that simulates a function expecting a specific enum type
        def requires_mean_type(mean_type):
            if not isinstance(mean_type, type(eCondMeanEnum.eUnknown)):
                raise TypeError("Expected eCondMeanEnum")
            return mean_type == eCondMeanEnum.eAr
        
        # Test with correct enum type
        self.assertTrue(requires_mean_type(eCondMeanEnum.eAr))
        self.assertFalse(requires_mean_type(eCondMeanEnum.eMa))
        
        # This is a bit of a hacky way to test for type errors in Python,
        # but it works for our purposes
        with self.assertRaises(TypeError):
            requires_mean_type(eCondVarEnum.eArch)  # Wrong enum type

if __name__ == '__main__':
    unittest.main()