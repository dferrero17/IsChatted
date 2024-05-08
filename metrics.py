import pandas as pd
from sklearn.metrics import confusion_matrix, accuracy_score, precision_score, recall_score


file_path = '/mnt/data/message-6.txt'
data = pd.read_csv(file_path, header=None, names=[
    "k", "alpha", "sizelimit", "filter", "isActuallySynthetic", 
    "PredictedSynthetic", "ratio_synthetic_human_bits", 
    "total_number_of_symbols", "time"])


data.head()



y_true = data['isActuallySynthetic']
y_pred = data['PredictedSynthetic']


cm = confusion_matrix(y_true, y_pred)


precision = precision_score(y_true, y_pred)
recall = recall_score(y_true, y_pred)
accuracy = accuracy_score(y_true, y_pred)


# Group data by different configurations and calculate metrics for each group
grouped_metrics = data.groupby(['k', 'alpha', 'sizelimit', 'filter']).apply(
    lambda df: pd.Series({
        'Precision': precision_score(df['isActuallySynthetic'], df['PredictedSynthetic']),
        'Recall': recall_score(df['isActuallySynthetic'], df['PredictedSynthetic']),
        'Accuracy': accuracy_score(df['isActuallySynthetic'], df['PredictedSynthetic'])
    })
).reset_index()

# Plotting line plots for each metric
fig, ax = plt.subplots(3, 1, figsize=(12, 18))

sns.lineplot(data=grouped_metrics, x='k', y='Precision', hue='filter', style='alpha', markers=True, ax=ax[0])
ax[0].set_title('Precision vs. k')
ax[0].set_ylabel('Precision')
ax[0].set_xlabel('k Value')

sns.lineplot(data=grouped_metrics, x='k', y='Recall', hue='filter', style='alpha', markers=True, ax=ax[1])
ax[1].set_title('Recall vs. k')
ax[1].set_ylabel('Recall')
ax[1].set_xlabel('k Value')

sns.lineplot(data=grouped_metrics, x='k', y='Accuracy', hue='filter', style='alpha', markers=True, ax=ax[2])
ax[2].set_title('Accuracy vs. k')
ax[2].set_ylabel('Accuracy')
ax[2].set_xlabel('k Value')

plt.tight_layout()
plt.show()


