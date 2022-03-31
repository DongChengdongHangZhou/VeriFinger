#include <TutorialUtils.hpp>

#ifdef N_MAC_OSX_FRAMEWORKS
	#include <NCore/NCore.hpp>
	#include <NBiometricClient/NBiometricClient.hpp>
	#include <NBiometrics/NBiometrics.hpp>
	#include <NMedia/NMedia.hpp>
	#include <NLicensing/NLicensing.hpp>
#else
	#include <NCore.hpp>
	#include <NBiometricClient.hpp>
	#include <NBiometrics.hpp>
	#include <NMedia.hpp>
	#include <NLicensing.hpp>
#endif
#include <iostream>
#include <fstream>
#include <string>
#include<stdio.h>

using namespace std;
using namespace Neurotec;
using namespace Neurotec::Licensing;
using namespace Neurotec::Biometrics;
using namespace Neurotec::Biometrics::Client;

const NChar title[] = N_T("VerifyFinger");
const NChar description[] = N_T("Demonstrates fingerprint verification.");
const NChar version[] = N_T("12.2.0.0");
const NChar copyright[] = N_T("Copyright (C) 2016-2021 Neurotechnology");

int usage()
{
	cout << "usage:" << endl;
	cout << "\t" << title << " [reference image] [candidate image] " << endl;
	return 1;
}

static NSubject CreateSubject(const NStringWrapper& fileName, const NStringWrapper& subjectId)
{
	NSubject subject;
	subject.SetId(subjectId);
	NFinger finger;
	finger.SetFileName(fileName);
	subject.GetFingers().Add(finger);
	return subject;
}

int main(int argc, NChar **argv)
{
	OnStart(title, description, version, copyright, argc, argv);
	//=========================================================================
	// CHOOSE LICENCES !!!
	//=========================================================================
	// ONE of the below listed "licenses" lines is required for unlocking this sample's functionality. Choose licenses that you currently have on your device.
	// If you are using a TRIAL version - choose any of them.

	const NChar * licenses = { N_T("FingerMatcher,FingerExtractor") };
	//const NChar * licenses = { N_T("FingerMatcher,FingerClient") };
	//const NChar * licenses = { N_T("FingerFastMatcher,FingerFastExtractor") };

	//=========================================================================

	//=========================================================================
	// TRIAL MODE
	//=========================================================================
	// Below code line determines whether TRIAL is enabled or not. To use purchased licenses, don't use below code line.
	// GetTrialModeFlag() method takes value from "Bin/Licenses/TrialFlag.txt" file. So to easily change mode for all our examples, modify that file.
	// Also you can just set TRUE to "TrialMode" property in code.

	NLicenseManager::SetTrialMode(GetTrialModeFlag());
	cout << "Trial mode: " << NLicenseManager::GetTrialMode() << endl;

	//=========================================================================

	int total_identity_evaluated = 5;  // 120
	int sample_num_per_identity = 3; // 10
	try
	{
		// Obtain licenses
		if (!NLicense::Obtain(N_T("/local"), N_T("5000"), licenses))
		{
			NThrowException(NString::Format(N_T("Could not obtain licenses: {S}"), licenses)); 
		}

		for (int a = 1;a <= total_identity_evaluated;a++) {
			cout << a << endl;
			for (int b = 1;b <= sample_num_per_identity;b++) {
				string srcdir1 = "cfpose/"+std::to_string(a) + "_" + std::to_string(b) + ".jpg";
				cout << srcdir1 << endl;
				for (int c = a;c <= total_identity_evaluated;c++) {
					for (int d = 1;d <= sample_num_per_identity;d++) {
						if (((c == a) && (d > b)) || (c > a)) {
							string srcdir2 = "cfpose/" + std::to_string(c) + "_" + std::to_string(d) + ".jpg";

							// NSubject referenceSubject = CreateSubject(argv[1], argv[1]);
							// NSubject candidateSubject = CreateSubject(argv[2], argv[2]);
							NSubject referenceSubject = CreateSubject(srcdir1, srcdir1);
							NSubject candidateSubject = CreateSubject(srcdir2, srcdir2);
							NBiometricClient biometricClient;
							biometricClient.SetMatchingThreshold(30); // 48
							biometricClient.SetFingersMatchingSpeed(nmsHigh); //nmsLow
							NBiometricStatus status = biometricClient.Verify(referenceSubject, candidateSubject);
							if (status == nbsOk || status == nbsMatchNotFound)
							{
								ofstream outfile;
								outfile.open("data.txt", ios::app);
								float score = referenceSubject.GetMatchingResults().Get(0).GetScore();
								cout << "Image score " << score << ", verification ";
								outfile << a << "_" << b << " "<< c<<"_" <<d <<" " << score << "\n";
								outfile.close();
								if (status == nbsOk)
								{
									cout << "succeeded" << endl;
								}
								else
								{
									cout << "failed" << endl;
								}
							}
							else
							{
								cout << "Verification failed. Status: " << NEnum::ToString(NBiometricTypes::NBiometricStatusNativeTypeOf(), status) << endl;
								return -1;
							}
						}
					}
				}
			}
		}
	}
	
	catch (NError& ex)
	{
		return LastError(ex);
	}
	OnExit();
	return 0;
}
