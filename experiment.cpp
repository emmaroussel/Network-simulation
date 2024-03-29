#include "experiment.h"

Experiment::Experiment() : network(new Network())
{
    // Create the neurons in the network
    network->create();
}

Experiment::~Experiment()
{
  delete network;
}

Network* Experiment::getNetwork() const {
  return network;
}

void Experiment::connect() {
  size_t nb_neurons(network->getNbNeurons());
  assert(nb_neurons > 0);

  /*
   * For each neuron, we ramdomly choose 1000 excitatory neurons and 250
   * inhibitory neurons that will target this particular neuron.
   */
  static random_device rd;
  static mt19937 gen(rd());
  static uniform_int_distribution<> distribEx(indexFirstExcitatoryNeuron, indexLastExcitatoryNeuron);
  static uniform_int_distribution<> distribIn(indexFirstInhibitoryNeuron, indexLastInhibitoryNeuron);
  //these indexes are found in the Constants.hpp file

  // We initialize all the connexions of the neurons in the network
  for (size_t i(0); i < nb_neurons; ++i) {

  // This neuron will be targeted by C_EXCI connexions from excitatory neurons
  for (size_t j(0); j < C_EXCI; ++j) {
    int random_index(distribEx(gen));
    network->addConnexion(random_index, i);
    /*
     * Neuron in position i of the vector all_neurons_ (in Network) have a
     * synaptic connexion to neuron in position random_index :
     * random_index neuron -> i neuron
     * (i is a post-synaptic neuron of neuron random_index)
     */
  }

    // This neuron will be targeted by C_INHI connexions from inhibitory neurons
    for (size_t k(0); k < C_INHI; ++k) {
      int random_index(distribIn(gen));
      network->addConnexion(random_index, i);
    }
  }

  cout << "Network connected." << endl;
}


void Experiment::run(long simulation_time) {
  cout << "Updating.." << endl;
  network->updateNetwork(simulation_time);
  cout << "End update" << endl;
}


void Experiment::save_spikes(string file, long simulation_time) {
  ofstream out;
  out.open(file);
  out.clear();

     if (out.fail()) {
        cerr << "Error opening the file" << endl;

     } else {

       vector<double> spikeTimes(network->getSpikeTimes());
       vector<size_t> neurons_indexes(network->getNeuronsIndexes());
       double spike_size(spikeTimes.size());

       assert(spike_size == neurons_indexes.size());

       cout << "Firing rate : "<< spike_size/(simulation_time/H) << "Hz" << endl;

        for (size_t i(0); i < spike_size; ++i) {
             out << spikeTimes[i]*H*0.01 << '\t'<< neurons_indexes[i] << '\n';
            // spikeTimes[i]*H is in ms
            // spikeTimes[i]*H*0.01 : a value of 1 is the file corresponds to 100ms
            // (so 1ms is represented by 0.01) -> we multiply by this so as to have the times
            // correlated to the ones asked by the graph simulation site used to plot the graphs
            //(cs116-plot.antoinealb.net)
        }
        out.close();
     }
}
