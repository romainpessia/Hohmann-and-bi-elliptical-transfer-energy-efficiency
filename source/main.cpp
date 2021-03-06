/****************************************************************
* HOHMANN AND BI-ELLIPTIC TRANSFER ENERGY EFFICIENCY CALCULATOR *
* Author: Romain Pessia                                         *
* Date: 2017/10/24                                              *
* License: GNU GPLv3 (see repo)                                 *
****************************************************************/

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <math.h>

using namespace std;

int main(int argc, char* argv[])
{
	//__________DEFINING VARIABLES__________

	// Minimal ratio desired
	double min_r21 = 11.;

	// Maximum ratio desired
	double max_r21 = 17.;
	
	// Number of samples of orbit radius ratio (r21) to take between the minimum and the maximum
	uint32_t num_samples = 100;

	// Vector of transfer radii ratio to test for the bi-elliptic transfer
	vector<double> transfer_radii_ratios =
	{ 5., 10., 15.58,  20., 40., 60., 80., 100., 150., 200., 500., 1000., 10000. };


	// Boolean variable for short file naming (set to true if no two transfer radii ratio have the same 
	// floored value)
	bool short_filename = true;

	if (argc > 5)
	{
		min_r21 = stod(argv[0]);
		max_r21 = stod(argv[1]);
		num_samples = stoi(argv[2]);
		short_filename = stoi(argv[3]);
		transfer_radii_ratios.clear();
		for (int32_t i = 4; i < argc - 1; ++i)
			transfer_radii_ratios.push_back(stod(argv[i]));
	}
	try
	{
		if (num_samples <= 1)
		{
			throw("The number of samples must be at least equal to 2.");
		}
		if (max_r21 <= min_r21)
		{
			throw("The maximum ratio between r2 and r1 must be greater than the minimum ratio.");
		}
		if (min_r21 <= 0.)
		{
			throw("The ratio between r2 and r1 must be non-negative.");
		}

		if (transfer_radii_ratios.empty())
		{
			throw("You must input at least one transfer radii ratio.");
		}
		for (auto ratio : transfer_radii_ratios)
		{
			if (ratio <= 0)
				throw("The ratios between rm and r1 must all be non-negative.");
		}
	}
	catch (string error)
	{
		cout << "Error: " << error;
		return 1;
	}
	
	//_________CALCULATING ENERGY EFFICIENCY__________

	// Ratio of the final orbit radius over the initial orbit radius (dimensionless)
	double r21 = 0.;

	// Hohmann transfer energy efficiency
	vector<vector<double>>hohmann_energy_efficiencies;
	for (uint32_t sample = 0; sample < num_samples; ++sample)
	{
		r21 = min_r21 + sample * (max_r21 - min_r21) / (num_samples-1);
		// For the Hohmann transfer, the energy efficiency can be easily calculated directly
		double energy_efficiency = sqrt(2 / (1 + 1 / r21)) - 1 + 1 / sqrt(r21) * (1 - sqrt(2 / (1 + r21)));
		hohmann_energy_efficiencies.push_back({r21, energy_efficiency});
	}

	// Exporting data
	// All (r21, EE) points for the Hohmann transfer
	ofstream output_file_hohmann;
	output_file_hohmann.open("output_hohmann.txt");
	for (uint32_t sample = 0; sample < num_samples; ++sample)
	{
		output_file_hohmann << "(" << hohmann_energy_efficiencies[sample][0]
			<< "," << hohmann_energy_efficiencies[sample][1] << ")";
	}
	output_file_hohmann.close();

	// Bi-elliptic transfer energy efficiency
	// Calculation for each transfer radii ratio
	for (double rm1 : transfer_radii_ratios)
	{
		vector<vector<double>>bi_elliptic_energy_efficiencies;
		bi_elliptic_energy_efficiencies.clear();
		for (uint32_t sample = 0; sample < num_samples; ++sample)
		{
			r21 = min_r21 + sample * (max_r21 - min_r21) / (num_samples - 1);
			// We calculate the energy efficiency directly here too
			double energy_efficiency = abs(sqrt(2-2/(1+rm1))-1)+abs(sqrt(2/rm1 -2/(r21+rm1))
				- sqrt(2/rm1 -2/(1+rm1)))+abs( - 1/sqrt(r21) + sqrt(2/r21-2/(r21+rm1)));
			bi_elliptic_energy_efficiencies.push_back({ r21, energy_efficiency });
		}
		// Exporting data
		// All (r21, energy_efficiency) points for the bi-elliptic transfer
		// (with the current transfer radii ratio)
		ofstream output_file_bi_elliptic;
		string file_name;
		if(short_filename)
			file_name = "output_bi_elliptic" + to_string(static_cast<uint32_t>(rm1)) + ".txt";
		else
			file_name = "output_bi_elliptic" + to_string(rm1) + ".txt";
		output_file_bi_elliptic.open(file_name);
		for (uint32_t i = 0; i < num_samples; ++i)
		{
			output_file_bi_elliptic << "(" << bi_elliptic_energy_efficiencies[i][0]
				<< "," << bi_elliptic_energy_efficiencies[i][1] << ")";
		}
		output_file_bi_elliptic.close();
	}
	
	return 0;
}