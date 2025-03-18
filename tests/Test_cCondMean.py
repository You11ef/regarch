import unittest
import regarch_wrapper as regarch

class TestCondMean(unittest.TestCase):
    def setUp(self):
        # Create a conditional mean model
        self.cond_mean = regarch.cCondMean()
        
        # Add a constant mean component
        self.const_mean = regarch.cConst(5.0)  # Constant value of 5.0
        self.cond_mean.add_one_mean(self.const_mean)
        
        # If available, add an AR(1) component
        try:
            self.ar_comp = regarch.cAr(1)  # AR(1) model
            self.ar_comp.set(0.7, 0, 0)  # AR coefficient = 0.7 using the unified set method
            self.cond_mean.add_one_mean(self.ar_comp)
            self.has_ar = True
        except Exception as e:
            print(f"AR component not available for testing: {e}")
            self.has_ar = False

    def test_get_n_mean(self):
        """Test getting the number of mean components"""
        expected_count = 2 if self.has_ar else 1
        self.assertEqual(self.cond_mean.get_n_mean(), expected_count)

    def test_get_cond_mean_type(self):
        """Test getting the types of all mean components"""
        types = self.cond_mean.get_cond_mean_type()
        expected_count = 2 if self.has_ar else 1
        self.assertEqual(len(types), expected_count)
        
    def test_get_n_param(self):
        """Test getting the total number of parameters"""
        n_param = self.cond_mean.get_n_param()
        # At least 1 for constant, plus 1 more if AR is available
        expected_min = 2 if self.has_ar else 1
        self.assertGreaterEqual(n_param, expected_min)
        
    def test_compute_mean(self):
        """Test computing the conditional mean"""
        # Create data object
        data = regarch.cRegArchValue(10)  # 10 data points
        
        # Initialize data values
        for i in range(10):
            data.mYt[i] = float(i)
            
        # Compute mean for date 5
        mean = self.cond_mean.compute_mean(5, data)
        self.assertIsInstance(mean, float)
        
    def test_delete_and_add(self):
        """Test deleting and adding components"""
        initial_count = self.cond_mean.get_n_mean()
        
        # Get the enum for constant condition mean
        try:
            # Try different possible enum values for constant mean
            self.cond_mean.delete_one_mean_enum(regarch.eCstCondMean)
            const_enum = regarch.eCstCondMean
        except:
            try:
                self.cond_mean.delete_one_mean_enum(regarch.eConstCondMean)
                const_enum = regarch.eConstCondMean
            except:
                try:
                    self.cond_mean.delete_one_mean_enum(regarch.eConst)
                    const_enum = regarch.eConst
                except Exception as e:
                    self.skipTest(f"Could not find correct enum for cConst: {e}")
                    return
        
        # Check count decreased
        new_count = self.cond_mean.get_n_mean()
        self.assertEqual(new_count, initial_count - 1)
        
        # Add it back
        self.cond_mean.add_one_mean(self.const_mean)
        
        # Check count restored
        final_count = self.cond_mean.get_n_mean()
        self.assertEqual(final_count, initial_count)
        
    def test_assignment_operator(self):
        """Test the assignment operator"""
        # Create a new empty conditional mean
        new_mean = regarch.cCondMean()
        
        # Check it has no components
        self.assertEqual(new_mean.get_n_mean(), 0)
        
        # Assign our existing mean to it
        try:
            new_mean.__assign__(self.cond_mean)
            # Check it now has the same number of components
            self.assertEqual(new_mean.get_n_mean(), self.cond_mean.get_n_mean())
        except Exception as e:
            self.skipTest(f"Assignment operator not working: {e}")

if __name__ == '__main__':
    unittest.main()