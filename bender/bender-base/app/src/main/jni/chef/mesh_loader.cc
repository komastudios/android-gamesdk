#include <vcg/complex/complex.h>

#include <vcg/complex/algorithms/update/bounding.h>
#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/complex/algorithms/update/normal.h>
#include <vcg/complex/algorithms/update/flag.h>
#include <vcg/complex/algorithms/local_optimization/tri_edge_collapse_quadric_tex.h>

// input output
#include <wrap/io_trimesh/import.h>
#include <wrap/io_trimesh/export.h>

// The class prototypes.
class MyVertex;
class MyEdge;
class MyFace;

struct MyUsedTypes: public vcg::UsedTypes<vcg::Use<MyVertex>::AsVertexType, vcg::Use<MyEdge>::AsEdgeType, vcg::Use<MyFace>::AsFaceType>{};

class MyVertex  : public vcg::Vertex< MyUsedTypes,
  vcg::vertex::VFAdj,
  vcg::vertex::Coord3f,
  vcg::vertex::Normal3f,
  vcg::vertex::Mark,
  vcg::vertex::BitFlags  >{
  };

class MyEdge : public vcg::Edge< MyUsedTypes> {};

typedef BasicVertexPair<MyVertex> VertexPair;

class MyFace    : public vcg::Face< MyUsedTypes,
  vcg::face::VFAdj,
  vcg::face::VertexRef,
  vcg::face::BitFlags,
  vcg::face::WedgeTexCoord2f,
  vcg::face::WedgeRealNormal3f,
  vcg::face::Normal3f> {};

// the main mesh class
class MyMesh    : public vcg::tri::TriMesh<std::vector<MyVertex>, std::vector<MyFace> > {};

class MyTriEdgeCollapseQTex: public TriEdgeCollapseQuadricTex< MyMesh, VertexPair, MyTriEdgeCollapseQTex, QuadricTexHelper<MyMesh> > {
            public:
            typedef  TriEdgeCollapseQuadricTex< MyMesh,  VertexPair, MyTriEdgeCollapseQTex, QuadricTexHelper<MyMesh> > TECQ;
            inline MyTriEdgeCollapseQTex(  const VertexPair &p, int i,vcg::BaseParameterClass *pp) :TECQ(p,i,pp){}
};


void MeshLoader(std::string &file_name)
{
    MyMesh mesh;

    int mask;
    int err = vcg::tri::io::ImporterOBJ<MyMesh>::Open(mesh, file_name.c_str(), mask);
    if (err)
    {
        printf("Unable to open mesh %s : '%s'\n", file_name.c_str(), vcg::tri::io::ImporterOBJ<MyMesh>::ErrorMsg(err));
        exit(-1);
    }
    printf("mesh loaded %d %d \n", mesh.vn, mesh.fn);

    TriEdgeCollapseQuadricTexParameter qparams;
    qparams.SetDefaultParams();
    qparams.NormalCheck = true;
    qparams.PreserveBoundary = true;
    qparams.PreserveTopology = true;

     int dup = vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(mesh);
     int unref = vcg::tri::Clean<MyMesh>::RemoveUnreferencedVertex(mesh);
     printf("Removed %i duplicate and %i unreferenced vertices from mesh \n", dup, unref);

    printf("Starting Reducing\n");

    vcg::tri::UpdateBounding<MyMesh>::Box(mesh);
    vcg::math::Quadric<double> QZero;
    QZero.SetZero();
    QuadricTexHelper<MyMesh>::QuadricTemp TD3(mesh.vert,QZero);
    QuadricTexHelper<MyMesh>::TDp3()=&TD3;
    std::vector<std::pair<vcg::TexCoord2<float>,vcg::Quadric5<double> > > qv;

    QuadricTexHelper<MyMesh>::Quadric5Temp TD(mesh.vert,qv);
    QuadricTexHelper<MyMesh>::TDp()=&TD;

    // decimator initialization
    vcg::LocalOptimization<MyMesh> DeciSession(mesh, &qparams);

    int t1 = clock();
    DeciSession.Init<MyTriEdgeCollapseQTex>();
    int t2 = clock();
    printf("Initial Heap Size %i\n", int(DeciSession.h.size()));

    DeciSession.SetTimeBudget(0.5f);
    DeciSession.SetTargetVertices(65536);

    while (DeciSession.DoOptimization() && mesh.vn > 65536)
        printf("Current Mesh size %7i \r", mesh.vn);

    DeciSession.Finalize<MyTriEdgeCollapseQTex>();
    int t3 = clock();
    printf("mesh  %d %d Error %g \n", mesh.vn, mesh.fn, DeciSession.currMetric);
    printf("\nCompleted in (%5.3f+%5.3f) sec\n", float(t2 - t1) / CLOCKS_PER_SEC, float(t3 - t2) / CLOCKS_PER_SEC);

    mask |= vcg::tri::io::Mask::IOM_VERTNORMAL;
    vcg::tri::io::ExporterOBJ<MyMesh>::Save(mesh, "testOut.obj", mask);
}
