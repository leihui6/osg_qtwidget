#include "cloud_io.h"

cloud_io::cloud_io()
{

}

bool cloud_io::load_point_cloud_txt(const QString & filename, std::vector<point_3d>& points)
{
    points.clear();

    std::ifstream ifile(filename.toStdString(), std::ios::in);

    if (!ifile.is_open())
    {
        return false;
    }
    std::string line;

    point_3d point;

    while (std::getline(ifile, line))
    {
        if (line.size() < 1)
        {
            continue;
        }

        if (line[0] == '#')
        {
            continue;
        }

        std::stringstream s(line);

        float tmp = 0.0, value[9] = { 0 };

        int i = 0;

        while (s >> tmp)
        {
            value[i] = tmp;

            i++;
        }

        point.set_xyz(value[0]/1000, value[1]/1000, value[2]/1000);
        point.set_nxyz(value[3], value[4], value[5]);
        point.set_rgb(value[6], value[7], value[8]);

        points.push_back(point);
    }

    //std::cout << "load_point_cloud_txt() " << filename << " size=" << points.size() << std::endl;

    return true;
}
