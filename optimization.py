import optuna
from test import get_score
import joblib
import argparse
import datetime
import os

parser = argparse.ArgumentParser()
parser.add_argument('--resume', type=str, default=None) 
parser.add_argument('--trials', type=int, default=10) 
parser.add_argument('--result-path', type=str, default='./study') 

def objective(trial):
    startTemp = trial.suggest_float("startTemp", 0.1, 100.0)
    #endTemp = trial.suggest_float("endTemp", 0.1, 100.0)
    endTemp = trial.suggest_float("endTemp", 0.1, startTemp)
    args = [startTemp, endTemp]
    return -1.0 * get_score(debug=False, args=args)

def main(args):
    if args.resume is not None:
        study = joblib.load(args.resume)
    else:
        #study = optuna.create_study()
        study = optuna.create_study(pruner=optuna.pruners.HyperbandPruner)
    study.optimize(objective, n_trials=args.trials)

    if not os.path.exists(args.result_path):
        os.mkdir(args.result_path)
    outfile = os.path.join(args.result_path, 'study_' + datetime.datetime.now().strftime('%Y-%m-%d_%H-%M-%S') + '.pkl')
    joblib.dump(study, outfile)

    best_params = study.best_params
    print(best_params)

if __name__ == '__main__':
    args = parser.parse_args()
    main(args)
