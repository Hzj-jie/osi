
#include <iostream>
#include "../../../envs/deploys.hpp"
#include "../../../envs/os.hpp"
using namespace std;

int main()
{
#ifdef BOOST_WINDOWS_API
#define p(x) wcout << #x << '\t' << deploys.x() << endl;
#else
#define p(x) cout << #x << '\t' << deploys.x() << endl;
#endif
    p(service_name);
    p(deploys_folder);
    p(apps_folder);
    p(counter_folder);
    p(data_folder);
    p(log_folder);
    p(service_data_folder);
    p(temp_folder);
    p(application_info_output_filename);
#undef p
}

