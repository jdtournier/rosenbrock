#include <iostream>
#include <termviz.h>


template <typename ValueType>
inline ValueType pow2 (const ValueType x) { return x*x; }

inline double Rosenbrock (double x, double y, const double a, const double b)
{
  return pow2 (a-x) + b * pow2 (y-pow2(x));
}


template <class ImageType>
class Overlay {
  public:
    Overlay (const ImageType& main, double min, double max, termviz::ColourMap cmap = termviz::gray()) :
      overlay (main.width(), main.height()),
      pix (main, min, max, cmap.size()),
      cmap (cmap) { }

    int width () const { return pix.width(); }
    int height () const { return pix.height(); }
    termviz::ctype operator() (int x, int y) const {
      return overlay(x,y) ? overlay (x,y) : pix (x,y);
    }


    termviz::ctype add_colours (const termviz::ColourMap& colours) {
      termviz::ctype first = cmap.size();
      cmap.insert (cmap.end(), colours.begin(), colours.end());
      return first;
    }

    void dot (double xpos, double ypos, double size, int colour_index)
    {
      ypos = pix.height() - ypos;
      for (int y = std::max (ypos-size,0.0); y < std::min(ypos+size+1.0,pix.height()-1.0); ++y)
        for (int x = std::max (xpos-size,0.0); x < std::min(xpos+size+1.0,pix.width()-1.0); ++x)
          if (pow2(x-xpos)+pow2(y-ypos) <= pow2(size))
            overlay(x,y) = colour_index;
    }

    const termviz::ColourMap& colourmap () const { return cmap; }

    termviz::Image<termviz::ctype> overlay;

  private:
    termviz::Rescale<ImageType> pix;
    termviz::ColourMap cmap;
};







int main (int argc, char ** argv)
{
  const std::vector<std::string> args (argv, argv+argc);

  double a = 1.0, b = 100.0;
  if (args.size() > 2) {
    a = std::stod (args[1]);
    b = std::stod (args[2]);
  }

  termviz::Image<float> canvas (256, 256);
  const double xmin = -2.0, xmax = 2.0;
  const double ymin = -2.0, ymax = 2.0;

  for (int y = 0; y < canvas.height(); ++y) {
    double ypos = ymax - y*(ymax-ymin)/(canvas.height()-1);
    for (int x = 0; x < canvas.height(); ++x) {
      double xpos = xmin + x*(xmax-xmin)/(canvas.width()-1);
      canvas(x,y) = std::log (Rosenbrock (xpos,ypos, a, b));
    }
  }

  Overlay overlay (canvas, -10, 5);
  const int red = overlay.add_colours ({ { 100, 0, 0 } });


  overlay.overlay.clear();
  overlay.dot (150, 100, 2, red);

  std::cout << std::format ("plot of Rosenbrock function with parameters a = {}, b = {}:\n", a, b);
  termviz::imshow (overlay, overlay.colourmap());

  return 0;
}

