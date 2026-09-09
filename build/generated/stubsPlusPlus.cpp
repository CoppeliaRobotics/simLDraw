
// include this file after calling the SIM_PLUGIN(...) macro

SIM_DLLEXPORT void simLDraw_import_(import__in *in, import__out *out)
{
    sim::plugin->import_(in, out);
}

