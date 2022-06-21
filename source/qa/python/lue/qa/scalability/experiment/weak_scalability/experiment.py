from .. import experiment
from .. import shape
import os.path


class Experiment(experiment.Experiment):

    def __init__(self,
            data):

        super(Experiment, self).__init__(
            data,
            "weak_scalability",
            data.get(
                "description",
                "Weak scaling experiment of {}, "
                "relating runtime to an increasing number of workers, "
                "while keeping the problem size per worker constant"
                    .format(os.path.basename(data["command_pathname"]))))

        self.from_json(data)

    def from_json(self,
            data):
        self.array = shape.Shape(data["array"])
        self.partition = shape.Shape(data["partition"])

    def to_json(self):
        result = super(Experiment, self).to_json()
        result["array"] = self.array.to_json()
        result["partition"] = self.partition.to_json()

        return result

    def benchmark_result_pathname(self,
            result_prefix,
            cluster_name,
            scenario_name,
            nr_workers,
            extension):

        return os.path.join(
            self.workspace_pathname(result_prefix, cluster_name, scenario_name),
            "{}.{}".format(nr_workers, extension))
